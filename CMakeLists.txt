cmake_minimum_required(VERSION 3.20)

project(ps2-engine LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(glad STATIC extern/glad/src/gl.c)

target_include_directories(glad PUBLIC extern/glad/include)

add_executable(ps2-engine src/main.cpp)

target_include_directories(ps2-engine PRIVATE src)

find_package(SDL2 CONFIG REQUIRED)

target_link_libraries(ps2-engine PRIVATE glad SDL2::SDL2 SDL2::SDL2main)

add_executable(ps2-tests tests/test_math.cpp)

target_include_directories(ps2-tests PRIVATE src)
