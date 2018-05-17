cmake_minimum_required(VERSION 3.10)


project(TrainerFramework)
set(CMAKE_CXX_STANDARD 17)
set(${CMAKE_CXX_FLAGS} "${CMAKE_CXX_FLAGS} -Wall -Wextra")


# Main sources
set(SOURCES "src/main.cpp"
            "src/Console.h" "src/Console.cpp"
            "src/Hotkeys.h" "src/Hotkeys.cpp"
            "src/mem/Memory.h"
            "src/mem/Process.h"
            "src/script/Script.h"
            "src/script/Script.cpp")

# Set the target architecture here
set(TARGET_ARCH x86)


# Optimization and Debug flags based on build type
if(${CMAKE_BUILD_TYPE} MATCHES "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast")
elseif(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -O0")
endif()


# OS Specific Code
if (WIN32)
    set(SOURCES "${SOURCES}" "src/platform/win32/mem/Process.cpp"
                             "src/platform/win32/mem/Memory.cpp")
else()
    message(FATAL_ERROR "Platform not supported.")
endif()


# Architecture Specific Code
if (${TARGET_ARCH} MATCHES x86)
    message(STATUS "Targeting x86")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
    set(SOURCES "${SOURCES}" "src/platform/win32/script/Compile.cpp")
elseif(${TARGET_ARCH} MATCHES x64)
    message(STATUS "Targeting x64")
else()
    message(FATAL_ERROR "Architecture ${TARGET_ARCH} not supported.")
endif()


add_executable(output ${SOURCES})
