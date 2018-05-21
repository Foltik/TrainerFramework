#include "../../../mem/Process.h"

#include <Windows.h>
#include <TlHelp32.h>

Process::Process(const std::string& processName) {
    HANDLE tHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(entry);

    if (Process32First(tHandle, &entry)) {
        do {
            if (!strcmp(processName.data(), entry.szExeFile)) {
                CloseHandle(tHandle);
                nativeHandle = reinterpret_cast<uintptr_t>(OpenProcess(PROCESS_ALL_ACCESS, false, entry.th32ProcessID));
                valid = true;
                return;
            }
        } while (Process32Next(tHandle, &entry));
    }
    valid = false;
}

Process::~Process() {
    if (valid)
        CloseHandle(reinterpret_cast<HANDLE>(nativeHandle));
}

uintptr_t Process::getHandle() const {
    return nativeHandle;
}

Process::operator bool() const {
    return valid;
}


