package main

import (
	"bytes"
	"encoding/json"
	"flag"
	"log"
	"math/rand"
	"net/http"
	"time"

	"github.com/pion/webrtc/v4"
)

type ptzCommand struct {
	ID      uint64 `json:"id"`
	Command string `json:"command"`
	Value   int32  `json:"value"`
}

var ptzCommands = []string{"pan_left", "pan_right", "tilt_up", "tilt_down", "zoom_in", "zoom_out", "stop"}

func main() {
	signalURL := flag.String("signal", "http://localhost:8080", "Server signaling base URL")
	stun := flag.String("stun", "stun:stun.l.google.com:19302", "STUN server URL")
	interval := flag.Duration("interval", 1*time.Second, "Interval between PTZ commands")
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

		dc.OnOpen(func() {
			log.Println("data channel opened, starting PTZ command sender")
			go sendPTZCommands(dc, *interval)
		})

		dc.OnMessage(func(msg webrtc.DataChannelMessage) {
			log.Printf("received message (ignored): %s", string(msg.Data))
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

func sendPTZCommands(dc *webrtc.DataChannel, interval time.Duration) {
	ticker := time.NewTicker(interval)
	defer ticker.Stop()

	var id uint64

	for range ticker.C {
		id++
		cmd := ptzCommand{
			ID:      id,
			Command: ptzCommands[rand.Intn(len(ptzCommands))],
			Value:   rand.Int31n(101), // 0-100
		}

		data, err := json.Marshal(cmd)
		if err != nil {
			log.Printf("failed to marshal command: %v", err)
			continue
		}

		log.Printf("Sending PTZ command: ID=%d, Command=%s, Value=%d", cmd.ID, cmd.Command, cmd.Value)

		if err := dc.Send(data); err != nil {
			log.Printf("failed to send: %v", err)
		}
	}
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
