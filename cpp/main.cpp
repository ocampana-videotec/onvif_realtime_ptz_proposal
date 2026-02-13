#include <chrono>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>

#include <grpcpp/grpcpp.h>

#include "latency.grpc.pb.h"

namespace {

int64_t now_unix_ns() {
  auto now = std::chrono::system_clock::now();
  auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch());
  return ns.count();
}

}  // namespace

int main(int argc, char** argv) {
  std::string grpc_addr = "localhost:50051";
  int interval_ms = 1000;
  std::string csv_path = "rtt_stats.csv";

  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg == "--grpc_addr" && i + 1 < argc) {
      grpc_addr = argv[++i];
    } else if (arg == "--interval_ms" && i + 1 < argc) {
      interval_ms = std::stoi(argv[++i]);
    } else if (arg == "--csv" && i + 1 < argc) {
      csv_path = argv[++i];
    }
  }

  auto channel = grpc::CreateChannel(grpc_addr, grpc::InsecureChannelCredentials());
  auto stub = latency::LatencyService::NewStub(channel);

  grpc::ClientContext context;
  auto stream = stub->Measure(&context);
  if (!stream) {
    std::cerr << "Failed to create gRPC stream" << std::endl;
    return 1;
  }

  std::ofstream csv(csv_path, std::ios::out | std::ios::trunc);
  if (!csv.is_open()) {
    std::cerr << "Failed to open CSV file: " << csv_path << std::endl;
    return 1;
  }
  csv << "id,sent_at_ns,client_received_at_ns,server_received_at_ns,"
         "webrtc_rtt_ns,end_to_end_rtt_ns,client_received_to_server_ns"
      << std::endl;

  std::atomic<bool> done{false};
  std::thread reader([&]() {
    latency::LatencyResponse response;
    while (stream->Read(&response)) {
      int64_t now_ns = now_unix_ns();
      int64_t end_to_end_ns = now_ns - response.sent_at_ns();
      int64_t webrtc_rtt_ns = response.rtt_ns();
      int64_t client_to_server_ns = response.server_received_at_ns() - response.client_received_at_ns();
      std::cout << "id=" << response.id()
                << " webrtc_rtt_ns=" << webrtc_rtt_ns
                << " end_to_end_rtt_ns=" << end_to_end_ns
                << " client_received_at_ns=" << response.client_received_at_ns()
                << " server_received_at_ns=" << response.server_received_at_ns()
                << std::endl;
      csv << response.id() << ','
          << response.sent_at_ns() << ','
          << response.client_received_at_ns() << ','
          << response.server_received_at_ns() << ','
          << webrtc_rtt_ns << ','
          << end_to_end_ns << ','
          << client_to_server_ns
          << std::endl;
    }
    done = true;
  });

  uint64_t id = 1;
  while (!done) {
    latency::LatencyRequest request;
    request.set_id(id++);
    request.set_sent_at_ns(now_unix_ns());
    if (!stream->Write(request)) {
      break;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms));
  }

  stream->WritesDone();
  reader.join();
  grpc::Status status = stream->Finish();
  if (!status.ok()) {
    std::cerr << "gRPC stream error: " << status.error_message() << std::endl;
    return 1;
  }

  return 0;
}
