#include <iostream>
#include <string>
#include <thread>

#include <grpcpp/grpcpp.h>

#include "latency.grpc.pb.h"

int main(int argc, char** argv) {
  std::string grpc_addr = "localhost:50051";

  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg == "--grpc_addr" && i + 1 < argc) {
      grpc_addr = argv[++i];
    }
  }

  auto channel = grpc::CreateChannel(grpc_addr, grpc::InsecureChannelCredentials());
  auto stub = ptz::PTZService::NewStub(channel);

  grpc::ClientContext context;
  auto stream = stub->SendCommand(&context);
  if (!stream) {
    std::cerr << "Failed to create gRPC stream" << std::endl;
    return 1;
  }

  std::cout << "Connected to PTZ server at " << grpc_addr << std::endl;
  std::cout << "Listening for PTZ commands..." << std::endl;

  ptz::PTZCommand command;
  while (stream->Read(&command)) {
    std::cout << "PTZ Command Received: ID=" << command.id()
              << ", Command=" << command.command()
              << ", Value=" << command.value()
              << std::endl;
  }

  grpc::Status status = stream->Finish();
  if (!status.ok()) {
    std::cerr << "gRPC stream error: " << status.error_message() << std::endl;
    return 1;
  }

  return 0;
}
