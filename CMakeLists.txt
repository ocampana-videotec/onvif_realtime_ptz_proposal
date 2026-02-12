cmake_minimum_required (VERSION 3.14)

project (onvif_realtime_ptz_proposal
    VERSION 1.0.0 
    LANGUAGES CXX
)

# Kaitai generally works best with C++11 or higher (C++17 recommended)
set (CMAKE_CXX_STANDARD 23)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

find_library (KAITAI_RUNTIME kaitai_struct_cpp_stl_runtime)

find_program (KTS kaitai-struct-compiler REQURED)

subdirs (src)
