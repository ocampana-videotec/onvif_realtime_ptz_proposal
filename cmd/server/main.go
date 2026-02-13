package main

import (
	"encoding/json"
	"flag"
	"fmt"
	"io"
	"log"
	"net"
	"net/http"
	"runtime/debug"
	"sync"
	"time"

	"github.com/pion/webrtc/v4"
	"google.golang.org/grpc"
	"google.golang.org/grpc/codes"
	"google.golang.org/grpc/status"

	"latency_poc/internal/ptzpb"
)

type ptzCommand struct {
	ID      uint64 `json:"id"`
	Command string `json:"command"`
	Value   int32  `json:"value"`
}

type serverState struct {
	mutex sync.Mutex
	pc    *webrtc.PeerConnection
	data  *webrtc.DataChannel

	grpcMutex  sync.Mutex
	grpcStream ptzpb.PTZService_SendCommandServer
}

func main() {
	addr := flag.String("addr", ":8080", "HTTP listen address")
	grpcAddr := flag.String("grpc-addr", ":50051", "gRPC listen address")
	stun := flag.String("stun", "stun:stun.l.google.com:19302", "STUN server URL")
	flag.Parse()

	state := &serverState{}

	mux := http.NewServeMux()
	mux.HandleFunc("/offer", state.handleOffer(*stun))
	mux.HandleFunc("/answer", state.handleAnswer())

	httpServer := &http.Server{
		Addr:              *addr,
		Handler:           mux,
		ReadHeaderTimeout: 5 * time.Second,
		ReadTimeout:       10 * time.Second,
		WriteTimeout:      10 * time.Second,
		IdleTimeout:       60 * time.Second,
	}

	log.Printf("server listening on %s", *addr)
	go func() {
		if err := startGrpcServer(*grpcAddr, state); err != nil {
			log.Fatal(err)
		}
	}()
	log.Fatal(httpServer.ListenAndServe())
}

func startGrpcServer(addr string, state *serverState) error {
	listener, err := net.Listen("tcp", addr)
	if err != nil {
		return err
	}

	grpcServer := grpc.NewServer()
	ptzpb.RegisterPTZServiceServer(grpcServer, &ptzGrpcServer{state: state})
	log.Printf("gRPC listening on %s", addr)
	return grpcServer.Serve(listener)
}

func (s *serverState) handleOffer(stunURL string) http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {
		if r.Method != http.MethodGet {
			http.Error(w, "method not allowed", http.StatusMethodNotAllowed)
			return
		}

		s.mutex.Lock()
		defer s.mutex.Unlock()

		if s.pc != nil {
			http.Error(w, "session already created", http.StatusConflict)
			return
		}

		config := webrtc.Configuration{
			ICEServers: []webrtc.ICEServer{{URLs: []string{stunURL}}},
		}
		pc, err := webrtc.NewPeerConnection(config)
		if err != nil {
			http.Error(w, err.Error(), http.StatusInternalServerError)
			return
		}

		ordered := false
		maxRetransmits := uint16(0)
		dc, err := pc.CreateDataChannel("ptz", &webrtc.DataChannelInit{
			Ordered:        &ordered,
			MaxRetransmits: &maxRetransmits,
		})
		if err != nil {
			pc.Close()
			http.Error(w, err.Error(), http.StatusInternalServerError)
			return
		}

		dc.OnOpen(func() {
			log.Printf("data channel open")
		})

		dc.OnClose(func() {
			log.Printf("data channel closed")
		})

		dc.OnMessage(func(msg webrtc.DataChannelMessage) {
			var cmd ptzCommand
			if err := json.Unmarshal(msg.Data, &cmd); err != nil {
				log.Printf("failed to parse PTZ command: %v", err)
				return
			}
			log.Printf("Received PTZ command from client: ID=%d, Command=%s, Value=%d", cmd.ID, cmd.Command, cmd.Value)
			s.forwardPTZCommand(&cmd)
		})

		offer, err := pc.CreateOffer(nil)
		if err != nil {
			pc.Close()
			http.Error(w, err.Error(), http.StatusInternalServerError)
			return
		}
		if err = pc.SetLocalDescription(offer); err != nil {
			pc.Close()
			http.Error(w, err.Error(), http.StatusInternalServerError)
			return
		}

		gatherComplete := webrtc.GatheringCompletePromise(pc)
		<-gatherComplete

		localDesc := pc.LocalDescription()
		if localDesc == nil {
			pc.Close()
			http.Error(w, "failed to gather ICE", http.StatusInternalServerError)
			return
		}

		s.pc = pc
		s.data = dc

		w.Header().Set("Content-Type", "application/json")
		if err := json.NewEncoder(w).Encode(localDesc); err != nil {
			pc.Close()
			s.pc = nil
			s.data = nil
			return
		}
	}
}

func (s *serverState) handleAnswer() http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {
		if r.Method != http.MethodPost {
			http.Error(w, "method not allowed", http.StatusMethodNotAllowed)
			return
		}

		var answer webrtc.SessionDescription
		if err := json.NewDecoder(r.Body).Decode(&answer); err != nil {
			http.Error(w, err.Error(), http.StatusBadRequest)
			return
		}

		s.mutex.Lock()
		pc := s.pc
		s.mutex.Unlock()

		if pc == nil {
			http.Error(w, "no active session", http.StatusNotFound)
			return
		}

		if err := pc.SetRemoteDescription(answer); err != nil {
			http.Error(w, err.Error(), http.StatusInternalServerError)
			return
		}

		w.WriteHeader(http.StatusOK)
		_, _ = fmt.Fprintln(w, "ok")
	}
}

type ptzGrpcServer struct {
	ptzpb.UnimplementedPTZServiceServer
	state *serverState
}

func (g *ptzGrpcServer) SendCommand(stream ptzpb.PTZService_SendCommandServer) (err error) {
	defer func() {
		if recovered := recover(); recovered != nil {
			log.Printf("grpc handler panic: %v\n%s", recovered, debug.Stack())
			err = status.Error(codes.Internal, "grpc handler panic")
		}
	}()

	if stream == nil {
		return status.Error(codes.Internal, "nil grpc stream")
	}
	if g == nil || g.state == nil {
		return status.Error(codes.Internal, "nil grpc server state")
	}
	if err := g.state.setGrpcStream(stream); err != nil {
		return err
	}
	defer g.state.clearGrpcStream(stream)

	// Just keep the stream alive
	for {
		_, recvErr := stream.Recv()
		if recvErr == io.EOF {
			return nil
		}
		if recvErr != nil {
			return recvErr
		}
	}
}

func (s *serverState) setGrpcStream(stream ptzpb.PTZService_SendCommandServer) error {
	s.grpcMutex.Lock()
	defer s.grpcMutex.Unlock()
	if s.grpcStream != nil {
		if s.grpcStream.Context().Err() == nil {
			return status.Error(codes.ResourceExhausted, "grpc stream already active")
		}
	}
	s.grpcStream = stream
	log.Println("gRPC stream established")
	return nil
}

func (s *serverState) clearGrpcStream(stream ptzpb.PTZService_SendCommandServer) {
	s.grpcMutex.Lock()
	defer s.grpcMutex.Unlock()
	if s.grpcStream == stream {
		s.grpcStream = nil
		log.Println("gRPC stream cleared")
	}
}

func (s *serverState) forwardPTZCommand(cmd *ptzCommand) {
	s.grpcMutex.Lock()
	stream := s.grpcStream
	s.grpcMutex.Unlock()

	if stream == nil {
		log.Println("No gRPC stream available, command dropped")
		return
	}

	ptzCmd := &ptzpb.PTZCommand{
		Id:      cmd.ID,
		Command: cmd.Command,
		Value:   cmd.Value,
	}

	if err := stream.Send(ptzCmd); err != nil {
		log.Printf("Failed to forward PTZ command to gRPC: %v", err)
	} else {
		log.Printf("Forwarded PTZ command to C++ client: ID=%d, Command=%s, Value=%d", cmd.ID, cmd.Command, cmd.Value)
	}
}
