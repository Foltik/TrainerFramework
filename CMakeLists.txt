cmake_minimum_required(VERSION 3.10)
project(TrainerFramework)

set(CMAKE_CXX_STANDARD 17)

set(${CMAKE_CXX_FLAGS} "${CMAKE_CXX_FLAGS} -Wall -Wextra")

if(${CMAKE_BUILD_TYPE} MATCHES "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast -m32")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -O0 -m32")
endif()

add_executable(output src/main.cpp src/Console.h src/Console.cpp src/Memory.h src/Memory.cpp src/Hotkeys.cpp src/Hotkeys.h)