package main

import (
	"bytes"
	"encoding/json"
	"flag"
	"log"
	"net/http"
	"time"

	"github.com/pion/webrtc/v4"
)

type latencyMessage struct {
	ID                 uint64 `json:"id"`
	SentAtNs           int64  `json:"sent_at_ns"`
	ClientReceivedAtNs int64  `json:"client_received_at_ns,omitempty"`
}

func main() {
	signalURL := flag.String("signal", "http://localhost:8080", "Server signaling base URL")
	stun := flag.String("stun", "stun:stun.l.google.com:19302", "STUN server URL")
	flag.Parse()

	config := webrtc.Configuration{
		ICEServers: []webrtc.ICEServer{{URLs: []string{*stun}}},
	}

	pc, err := webrtc.NewPeerConnection(config)
	if err != nil {
		log.Fatal(err)
	}
	defer pc.Close()

	pc.OnDataChannel(func(dc *webrtc.DataChannel) {
		log.Printf("data channel received: %s", dc.Label())

		dc.OnMessage(func(msg webrtc.DataChannelMessage) {
			var payload latencyMessage
			if err := json.Unmarshal(msg.Data, &payload); err != nil {
				log.Printf("failed to parse message: %v", err)
				return
			}
			payload.ClientReceivedAtNs = time.Now().UnixNano()
			response, err := json.Marshal(payload)
			if err != nil {
				log.Printf("failed to marshal response: %v", err)
				return
			}
			if err := dc.Send(response); err != nil {
				log.Printf("send error: %v", err)
			}
		})
	})

	offer, err := fetchOffer(*signalURL)
	if err != nil {
		log.Fatal(err)
	}

	if err := pc.SetRemoteDescription(offer); err != nil {
		log.Fatal(err)
	}

	answer, err := pc.CreateAnswer(nil)
	if err != nil {
		log.Fatal(err)
	}
	if err := pc.SetLocalDescription(answer); err != nil {
		log.Fatal(err)
	}

	gatherComplete := webrtc.GatheringCompletePromise(pc)
	<-gatherComplete

	localDesc := pc.LocalDescription()
	if localDesc == nil {
		log.Fatal("failed to gather ICE")
	}

	if err := sendAnswer(*signalURL, localDesc); err != nil {
		log.Fatal(err)
	}

	select {}
}

func fetchOffer(baseURL string) (webrtc.SessionDescription, error) {
	resp, err := http.Get(baseURL + "/offer")
	if err != nil {
		return webrtc.SessionDescription{}, err
	}
	defer resp.Body.Close()

	if resp.StatusCode != http.StatusOK {
		return webrtc.SessionDescription{}, parseHTTPError(resp)
	}

	var offer webrtc.SessionDescription
	if err := json.NewDecoder(resp.Body).Decode(&offer); err != nil {
		return webrtc.SessionDescription{}, err
	}

	return offer, nil
}

func sendAnswer(baseURL string, answer *webrtc.SessionDescription) error {
	body, err := json.Marshal(answer)
	if err != nil {
		return err
	}

	resp, err := http.Post(baseURL+"/answer", "application/json", bytes.NewReader(body))
	if err != nil {
		return err
	}
	defer resp.Body.Close()

	if resp.StatusCode != http.StatusOK {
		return parseHTTPError(resp)
	}

	return nil
}

func parseHTTPError(resp *http.Response) error {
	return &httpError{status: resp.Status}
}

type httpError struct {
	status string
}

func (e *httpError) Error() string {
	return e.status
}
