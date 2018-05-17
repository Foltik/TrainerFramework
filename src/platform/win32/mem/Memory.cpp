#include "../../../mem/Memory.h"

#include "../../../mem/Process.h"

#include <Windows.h>

namespace {
    inline uint32_t protectToWin32MemFlags(uint8_t protect) {
        switch (protect) {
            case 0x0:
                return PAGE_NOACCESS;
            case 0x1:
                return PAGE_READONLY;
            case 0x2:
            case 0x3:
                return PAGE_READWRITE;
            case 0x4:
                return PAGE_EXECUTE;
            case 0x5:
                return PAGE_EXECUTE_READ;
            case 0x6:
            case 0x7:
                return PAGE_EXECUTE_READWRITE;
            default:
                return PAGE_EXECUTE_READWRITE;
        }
    }

    inline uint8_t win32MemFlagsToProtect(uint32_t winProtect) {
        switch(winProtect) {
            case PAGE_NOACCESS:
                return 0x0;
            case PAGE_READONLY:
                return Mem::Protect::READ;
            case PAGE_EXECUTE:
                return Mem::Protect::EXECUTE;
            case PAGE_READWRITE:
                return Mem::Protect::READ | Mem::Protect::WRITE;
            case PAGE_EXECUTE_READ:
                return Mem::Protect::READ | Mem::Protect::EXECUTE;
            case PAGE_EXECUTE_READWRITE:
                return Mem::Protect::READ | Mem::Protect::WRITE | Mem::Protect::EXECUTE;
            default:
                return 0x0;
        }
    }
}



bool Mem::writeData(const Process& proc, uintptr_t address, const void* data, size_t size) {
    return static_cast<bool>(WriteProcessMemory(reinterpret_cast<HANDLE>(proc.getHandle()),
                                                reinterpret_cast<void*>(address), data,
                                                size, nullptr));
}

bool Mem::readData(const Process& proc, uintptr_t address, void* out, size_t size) {
    return static_cast<bool>(ReadProcessMemory(reinterpret_cast<HANDLE>(proc.getHandle()),
                                               reinterpret_cast<LPCVOID>(address), out,
                                               size, nullptr));
}

bool Mem::writeBytes(const Process& proc, uintptr_t address, const std::vector<uint8_t>& bytes) {
    writeData(proc, address, static_cast<const void*>(bytes.data()), bytes.size());
}

std::vector<uint8_t> Mem::readBytes(const Process& proc, uintptr_t address, size_t size) {
    std::vector<uint8_t> data(size);
    readData(proc, address, static_cast<void*>(data.data()), size);
    return data;
}



std::vector<Mem::Page> Mem::enumRwPages(const Process& proc) {
    std::vector<Page> pages;

    SYSTEM_INFO info;
    GetSystemInfo(&info);
    for (uintptr_t address = reinterpret_cast<uintptr_t>(info.lpMinimumApplicationAddress);
         address <= reinterpret_cast<uintptr_t>(info.lpMaximumApplicationAddress);) {
        MEMORY_BASIC_INFORMATION page;
        VirtualQueryEx(reinterpret_cast<HANDLE>(proc.getHandle()), reinterpret_cast<LPCVOID>(address), &page, sizeof(page));

        if (page.State == MEM_COMMIT &&
            page.Type == MEM_PRIVATE &&
            page.Protect & (PAGE_READWRITE | PAGE_EXECUTE_READWRITE))
            pages.push_back({reinterpret_cast<uintptr_t>(page.BaseAddress),
                             page.RegionSize, win32MemFlagsToProtect(page.Protect)});

        address = reinterpret_cast<uintptr_t>(page.BaseAddress) + page.RegionSize;
    }

    return pages;
}

uintptr_t Mem::alloc(const Process& proc, size_t size, uint8_t protect) {
    return reinterpret_cast<uintptr_t>(VirtualAllocEx(reinterpret_cast<HANDLE>(proc.getHandle()), nullptr, size,
                                                      MEM_COMMIT, protectToWin32MemFlags(protect)));
}

void Mem::free(const Process& proc, uintptr_t address) {
    VirtualFreeEx(reinterpret_cast<HANDLE>(proc.getHandle()), reinterpret_cast<LPVOID>(address), 0, MEM_RELEASE);
}
