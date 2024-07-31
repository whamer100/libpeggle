cmake_minimum_required(VERSION 3.28)
project(libpeggle)

set(CMAKE_CXX_STANDARD 20)

add_library(libpeggle STATIC library.cpp)

set_target_properties(libpeggle PROPERTIES PUBLIC_HEADER "library.h")

configure_file("library.h" "${CMAKE_BINARY_DIR}/" COPYONLY)