# cmake_minimum_required(VERSION <specify CMake version here>)
project(StardewFisher)

set(CMAKE_CXX_STANDARD 17)

add_executable(StardewFisher src/main.cpp src/Console.h src/Console.cpp src/Memory.h src/Memory.cpp src/Hotkeys.cpp src/Hotkeys.h)