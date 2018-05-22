cmake_minimum_required(VERSION 3.10)

project(TrainerFramework)
set(CMAKE_CXX_STANDARD 17)

# Compiler Flags
if (MSVC)

else()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++")
endif()

# Main sources
set(SOURCES "src/main.cpp"
            "src/Console.h" "src/Console.cpp"
            "src/Hotkeys.h" "src/Hotkeys.cpp"
            "src/mem/Memory.h"
            "src/mem/Process.h"
            "src/script/AST.h"
            "src/script/Script.h" "src/script/Script.cpp"
            "src/script/Lexer.h" "src/script/Lexer.cpp"
            "src/script/Parser.h" "src/script/Parser.cpp"
            "src/script/Assembler.h" "src/script/Assembler.cpp"
            "src/script/Encode.h")

# Set the target architecture here
set(TARGET_ARCH x86)


# Optimization and Debug flags based on build type
if(${CMAKE_BUILD_TYPE} MATCHES "Release")
    if(MSVC)
    else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast")
    endif()
elseif(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    if(MSVC)
    else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -ggdb -O0")
    endif()
endif()


# OS Specific Code
if (WIN32)
    set(SOURCES "${SOURCES}" "src/platform/win32/mem/Process.cpp"
                             "src/platform/win32/mem/Memory.cpp"
                             "src/platform/win32/script/EncodeDirective.cpp"
                             "src/platform/win32/script/EncodeInstruction.cpp")
else()
    message(FATAL_ERROR "Platform not supported.")
endif()


# Architecture Specific Code
if (${TARGET_ARCH} MATCHES x86)
    message(STATUS "Targeting x86")
    if(MSVC)
    else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
    endif()
elseif(${TARGET_ARCH} MATCHES x64)
    message(STATUS "Targeting x64")
else()
    message(FATAL_ERROR "Architecture ${TARGET_ARCH} not supported.")
endif()


add_executable(output ${SOURCES})
