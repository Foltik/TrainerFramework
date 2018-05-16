cmake_minimum_required(VERSION 3.10)
project(TrainerFramework)

set(CMAKE_CXX_STANDARD 17)

set(${CMAKE_CXX_FLAGS} "${CMAKE_CXX_FLAGS} -Wall -Wextra")

if(${CMAKE_BUILD_TYPE} MATCHES "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast -m32")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -O0 -m32")
endif()

set(SOURCES "src/main.cpp"
            "src/Console.h" "src/Console.cpp"
            "src/Hotkeys.h" "src/Hotkeys.cpp"
            "src/Memory.h"
            "src/Process.h")

if (WIN32)
    set(SOURCES "${SOURCES}" "src/platform/win32/mem/Process.cpp"
                             "src/platform/win32/mem/Memory.cpp")
else()
    message(FATAL_ERROR "Platform not supported.")
endif()

add_executable(output ${SOURCES})
