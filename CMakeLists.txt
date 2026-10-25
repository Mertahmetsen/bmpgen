cmake_minimum_required(VERSION 4.3)
project(bmpgen C)
set(CMAKE_C_STANDARD 11)
set(RAYLIB_VERSION 6.0)

include(FetchContent)

FetchContent_Declare(
        cargs
        GIT_REPOSITORY https://github.com/likle/cargs.git
        GIT_TAG stable
)

FetchContent_Declare(
        raylib
        URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
        FIND_PACKAGE_ARGS ${RAYLIB_VERSION}
)
# Must be set before MakeAvailable so raylib skips building its examples
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(cargs raylib)

add_executable(bmpgen main.c)
target_link_libraries(bmpgen PRIVATE cargs raylib)