cmake_minimum_required(VERSION 3.20)

project(ps2-engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(ps2-engine src/main.cpp)

find_package(SDL2 CONFIG REQUIRED)

target_link_libraries(ps2-engine PRIVATE SDL2::SDL2 SDL2::SDL2main)


