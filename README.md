# WebRTC PTZ Command Forwarding (Pion)

Go server/client system that forwards PTZ (Pan-Tilt-Zoom) commands from a WebRTC client through a Go server to a C++ application via gRPC.

## Architecture

1. **Go WebRTC Client** - Generates random PTZ commands and sends them to the server via WebRTC data channel
2. **Go Server** - Receives PTZ commands from the WebRTC client and forwards them to the C++ app via gRPC
3. **C++ gRPC Client** - Receives and logs PTZ commands

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
go run ./cmd/client -signal http://localhost:8080 -interval 1s
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
  --go_out=internal/ptzpb --go_opt=paths=source_relative \
  --go-grpc_out=internal/ptzpb --go-grpc_opt=paths=source_relative \
  proto/latency.proto
```

The generated files (`latency.pb.go` and `latency_grpc.pb.go`) will be created in `internal/ptzpb/`.

After regeneration, rebuild the Go apps:

```bash
go mod tidy
go build -o bin/server ./cmd/server
go build -o bin/client ./cmd/client
```

## Run

Run order: start the Go server, then the C++ gRPC client, then the Go WebRTC client.

Start the server (HTTP signaling + gRPC):

```bash
cd latency_poc

go run ./cmd/server -addr :8080 -grpc-addr :50051
```

In another terminal, start the C++ client to listen for PTZ commands:

```bash
cd latency_poc/cpp

./build/latency_client --grpc_addr localhost:50051
```

In a third terminal, start the Go WebRTC client to send PTZ commands:

```bash
cd latency_poc

go run ./cmd/client -signal http://localhost:8080 -interval 1s
```

The client will send random PTZ commands (pan_left, pan_right, tilt_up, tilt_down, zoom_in, zoom_out, stop) with random values (0-100) at the configured interval.

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

Run the C++ app:

```bash
cd latency_poc/cpp

./build/latency_client --grpc_addr localhost:50051
```

The C++ client will log each received PTZ command with its ID, command name, and value.

## Command Line Options

### Go Server
- `-addr` - HTTP signaling address (default: `:8080`)
- `-grpc-addr` - gRPC listen address (default: `:50051`)
- `-stun` - STUN server URL (default: `stun:stun.l.google.com:19302`)

### Go Client
- `-signal` - Server signaling base URL (default: `http://localhost:8080`)
- `-stun` - STUN server URL (default: `stun:stun.l.google.com:19302`)
- `-interval` - Interval between PTZ commands (default: `1s`)

### C++ Client
- `--grpc_addr` - gRPC server address (default: `localhost:50051`)

## Notes
- The data channel is configured with `Ordered=false` and `MaxRetransmits=0` for minimal latency.
- The server accepts a single gRPC stream and WebRTC client at a time for simplicity.
- PTZ commands are: `pan_left`, `pan_right`, `tilt_up`, `tilt_down`, `zoom_in`, `zoom_out`, `stop`
- Command values are random integers from 0-100

