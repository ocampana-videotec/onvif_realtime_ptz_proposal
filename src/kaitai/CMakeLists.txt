add_custom_command (
	OUTPUT onvif_realtime_ptz.cpp onvif_realtime_ptz.h
	COMMAND ${KTS} -t cpp_stl onvif_realtime_ptz.ksy
	VERBATIM)

add_executable (kaitai_performance_evaluation	main.cpp
						onvif_realtime_ptz.cpp onvif_realtime_ptz.h)
target_link_libraries (kaitai_performance_evaluation PUBLIC ${KAITAI_RUNTIME})
