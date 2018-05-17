#pragma once

#include <string>

class Process {
public:
    Process(const std::string& processName);
    Process(const Process& rhs) = delete;
    ~Process();

    Process& operator=(const Process& other) = delete;

    uintptr_t getHandle() const;
    operator bool() const;

private:
    uintptr_t nativeHandle;
    bool valid;
};

