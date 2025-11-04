cmake_minimum_required(VERSION 4.0)
project(ElectricEraCodingChallenge)

set(CMAKE_CXX_STANDARD 20)
add_compile_definitions(SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")


add_executable(ElectricEraCodingChallenge main.cpp
        src/Library.h
        src/ChargingNetwork.h
        src/ChargingNetwork.cpp
        src/Library.cpp)
