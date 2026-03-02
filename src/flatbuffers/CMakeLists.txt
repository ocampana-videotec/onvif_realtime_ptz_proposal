add_custom_command (
	OUTPUT onvif_realtime_ptz_generated.h
	COMMAND ${FLATC} -c onvif_realtime_ptz.fbs
	VERBATIM)

add_executable (flatbuffers_performance_evaluation	main.cpp
							onvif_realtime_ptz_generated.h)

#target_link_libraries (kaitai_performance_evaluation PUBLIC ${KAITAI_RUNTIME})

#target_compile_options (kaitai_performance_evaluation PRIVATE -g)
