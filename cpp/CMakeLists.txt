cmake_minimum_required(VERSION 3.16)
project(latency_client LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Protobuf CONFIG QUIET)
if (NOT Protobuf_FOUND)
  find_package(Protobuf REQUIRED)
endif()

find_package(gRPC CONFIG QUIET)
if (NOT gRPC_FOUND)
  find_package(gRPC REQUIRED)
endif()

set(PROTO_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../proto)
set(PROTO_FILE ${PROTO_SRC_DIR}/latency.proto)
set(GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)

file(MAKE_DIRECTORY ${GENERATED_DIR})

set(PROTO_SRCS ${GENERATED_DIR}/latency.pb.cc)
set(PROTO_HDRS ${GENERATED_DIR}/latency.pb.h)
set(GRPC_SRCS ${GENERATED_DIR}/latency.grpc.pb.cc)
set(GRPC_HDRS ${GENERATED_DIR}/latency.grpc.pb.h)

if (TARGET gRPC::grpc_cpp_plugin)
  set(GRPC_CPP_PLUGIN $<TARGET_FILE:gRPC::grpc_cpp_plugin>)
else()
  find_program(GRPC_CPP_PLUGIN grpc_cpp_plugin)
endif()

if (NOT GRPC_CPP_PLUGIN)
  message(FATAL_ERROR "grpc_cpp_plugin not found. Install protobuf/grpc tools or set GRPC_CPP_PLUGIN.")
endif()

add_custom_command(
  OUTPUT ${PROTO_SRCS} ${PROTO_HDRS} ${GRPC_SRCS} ${GRPC_HDRS}
  COMMAND ${Protobuf_PROTOC_EXECUTABLE}
  ARGS --proto_path=${PROTO_SRC_DIR}
       --cpp_out=${GENERATED_DIR}
       --grpc_out=${GENERATED_DIR}
      --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN}
       ${PROTO_FILE}
  DEPENDS ${PROTO_FILE}
  VERBATIM
)

add_executable(latency_client
  main.cpp
  ${PROTO_SRCS}
  ${GRPC_SRCS}
)

target_include_directories(latency_client PRIVATE ${GENERATED_DIR})

target_link_libraries(latency_client
  gRPC::grpc++
  protobuf::libprotobuf
)
