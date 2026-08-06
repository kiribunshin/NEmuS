cmake_minimum_required(VERSION 4.0)
project(NES_Emulator)

set(CMAKE_CXX_STANDARD 20)

add_executable(NES_Emulator main.cpp
        cpu.h
        cpu.cpp
        bus.cpp
        bus.h)
