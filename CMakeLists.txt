cmake_minimum_required(VERSION 3.25)
project(LuxMipmapProcessor)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory("libs/vtflib")
include_directories("libs/vtflib/VTFLib")
include_directories("libs/stb")

add_executable(LuxMipmapProcessor src/main.cpp libs/stb/stb_image.h)
target_link_libraries(LuxMipmapProcessor
        vtflib
        )

