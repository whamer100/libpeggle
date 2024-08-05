cmake_minimum_required(VERSION 3.28)
project(libpeggle)

if(NOT CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    message(FATAL_ERROR "MSVC is required to build this project.")
endif()

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set(vendor_sources
        vendor/endianness.h
)

set(target_sources
        libpeggle.cpp
        peggleconfig.cpp
        iohelper.cpp
        logma.cpp
)
set(target_headers
        libpeggle.h
        iohelper.h
        logma.h
        binstream.h
        macros.h
)

add_library(${PROJECT_NAME} STATIC
        ${target_sources}
        ${target_headers}
        ${vendor_sources}
)

set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "libpeggle.h")

configure_file("libpeggle.h" "${CMAKE_BINARY_DIR}/" COPYONLY)

### TESTING APPLICATION GOES HERE ###

project(libpeggle_testing)

set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME} testing/main.cpp)
target_link_libraries(${PROJECT_NAME} libpeggle)

configure_file("testing/simple.pak" "${CMAKE_BINARY_DIR}/" COPYONLY)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/testing/simple
        ${CMAKE_CURRENT_BINARY_DIR}/simple
)
# file(COPY "testing/simple" DESTINATION "${CMAKE_BINARY_DIR}/")