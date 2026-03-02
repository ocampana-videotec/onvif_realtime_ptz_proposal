add_custom_command (
	OUTPUT onvif_realtime_ptz.pb.cc onvif_realtime_ptz.pb.h
	COMMAND ${PROTOC} --cpp_out=. onvif_realtime_ptz.proto
	VERBATIM)

add_executable (protobuffers_performance_evaluation	main.cpp
							onvif_realtime_ptz.pb.cc onvif_realtime_ptz.pb.h)

target_link_libraries (protobuffers_performance_evaluation PUBLIC ${PROTOBUF})

#target_compile_options (kaitai_performance_evaluation PRIVATE -g)
