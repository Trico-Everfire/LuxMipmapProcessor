cmake_minimum_required(VERSION 3.25)
project(LuxMipmapProcessor)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_RPATH_USE_ORIGIN TRUE)
set(CMAKE_INSTALL_RPATH $ORIGIN)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(VTFLIB_STATIC OFF CACHE BOOL "" FORCE)

add_subdirectory("libs/vtflib")

set_target_properties(
        vtflib PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
        LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
        RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

include_directories("libs/vtflib/VTFLib")
include_directories("libs/vtflib/thirdparty/")
include_directories("libs/stb")

add_executable(LuxMipmapProcessor src/main.cpp libs/stb/stb_image.h)
target_link_libraries(LuxMipmapProcessor PRIVATE
        vtflib
        )

