# WebRTC Data Channel Latency (Pion)

Go server/client measure WebRTC data channel latency and expose a gRPC service so a C++ app can drive the RTT path.

## Requirements
- Go 1.22+
- Network access to the configured STUN server (default: `stun:stun.l.google.com:19302`)
- C++17 toolchain, CMake, gRPC, and Protobuf (for the C++ app)

## Build

### Go Applications

Install dependencies and build:

```bash
cd latency_poc

go mod tidy
go build -o bin/server ./cmd/server
go build -o bin/client ./cmd/client
```

**Cross-compile for ARM64:**

```bash
GOOS=linux GOARCH=arm64 go build -o bin/server-arm64 ./cmd/server
GOOS=linux GOARCH=arm64 go build -o bin/client-arm64 ./cmd/client
```

Or run directly:

```bash
go run ./cmd/server -addr :8080 -grpc-addr :50051
go run ./cmd/client -signal http://localhost:8080
```

### Regenerate Protobuf Code (if proto changes)

Install protobuf compiler and Go plugins:

```bash
sudo apt-get install -y protobuf-compiler
go install google.golang.org/protobuf/cmd/protoc-gen-go@latest
go install google.golang.org/grpc/cmd/protoc-gen-go-grpc@latest
```

Ensure the Go plugin binaries are in your PATH:

```bash
export PATH="$PATH:$(go env GOPATH)/bin"
```

Generate Go code from `proto/latency.proto`:

```bash
cd latency_poc

protoc --proto_path=proto \
  --go_out=internal/latencypb --go_opt=paths=source_relative \
  --go-grpc_out=internal/latencypb --go-grpc_opt=paths=source_relative \
  proto/latency.proto
```

The generated files (`latency.pb.go` and `latency_grpc.pb.go`) will be created in `internal/latencypb/`.

After regeneration, rebuild the Go apps:

```bash
go mod tidy
go build -o bin/server ./cmd/server
go build -o bin/client ./cmd/client
```

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

**Cross-compile for ARM64:**

```bash
cmake -S . -B build-arm64 \
  -DCMAKE_SYSTEM_NAME=Linux \
  -DCMAKE_SYSTEM_PROCESSOR=aarch64 \
  -DCMAKE_C_COMPILER=aarch64-linux-gnu-gcc \
  -DCMAKE_CXX_COMPILER=aarch64-linux-gnu-g++
cmake --build build-arm64
```

Prerequisites for cross-compilation:

```bash
sudo apt-get install -y gcc-aarch64-linux-gnu g++-aarch64-linux-gnu
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
