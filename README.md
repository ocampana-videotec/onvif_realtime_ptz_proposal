# WebRTC Data Channel Latency (Pion)

Go server/client measure WebRTC data channel latency and expose a gRPC service so a C++ app can drive the RTT path.

## Requirements
- Go 1.22+
- Network access to the configured STUN server (default: `stun:stun.l.google.com:19302`)
- C++17 toolchain, CMake, gRPC, and Protobuf (for the C++ app)

## Run

Run order: start the Go server, then the Go WebRTC client, then the C++ gRPC client.

Start the server (HTTP signaling + gRPC):

```bash
cd latency_poc

go run ./cmd/server -addr :8080 -grpc-addr :50051
```

In another terminal, start the client:

```bash
cd latency_poc

go run ./cmd/client -signal http://localhost:8080
```

The server prints RTT measurements as replies arrive.

## C++ gRPC Client

Prerequisites:

```bash
sudo apt-get install -y protobuf-compiler libprotobuf-dev libgrpc++-dev protobuf-compiler-grpc
```


Build the C++ app:

```bash
cd latency_poc/cpp

cmake -S . -B build
cmake --build build
```

Run the C++ app (sends timestamps via gRPC, receives RTT responses):

```bash
cd latency_poc/cpp

./build/latency_client --grpc_addr localhost:50051 --interval_ms 1000
```

Output includes `webrtc_rtt_ns` (server-measured data-channel RTT) and `end_to_end_rtt_ns` (C++ → Go server → Go client → Go server → C++).

The C++ client also writes a CSV file (default `rtt_stats.csv`).

```bash
./build/latency_client --grpc_addr localhost:50051 --interval_ms 1000 --csv rtt_stats.csv
```

Plot the RTTs from CSV:

```bash
python3 scripts/plot_rtt.py --csv rtt_stats.csv --out rtt_plot.png
```

Plot RTT histograms:

```bash
python3 scripts/plot_histogram.py --csv rtt_stats.csv --out rtt_hist.png
```

## Notes
- The data channel is configured with `Ordered=false` and `MaxRetransmits=0` for minimal latency.
- The server accepts a single gRPC stream and WebRTC client at a time for simplicity.
