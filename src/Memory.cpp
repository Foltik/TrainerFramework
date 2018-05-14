#include "Memory.h"

#include <TlHelp32.h>
#include <iostream>

Memory::Memory(const std::string& processName) {
    if (attach(processName)) {
        std::cout << "Attached to " << processName.data() << std::endl;
    } else {
        std::cout << "Failed to attach to " << processName.data() << std::endl;
        system("pause");
    }
}

Memory::~Memory() {
    if (attached)
        detach();
}

HANDLE Memory::getHandle() const {
    return handle;
}

bool Memory::attach(const std::string& processName) {
    HANDLE tHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(entry);

    if (Process32First(tHandle, &entry)) {
        do {
            if (!strcmp(processName.data(), entry.szExeFile)) {
                pid = entry.th32ProcessID;
                handle = OpenProcess(PROCESS_ALL_ACCESS, false, pid);

                attached = true;
                CloseHandle(tHandle);

                findUsablePages();

                return true;
            }
        } while (Process32Next(tHandle, &entry));
    }
    return false;
}

void Memory::detach() {
    attached = false;
    CloseHandle(handle);
}

bool Memory::writeArrayOfBytes(uintptr_t address, std::vector<uint8_t>&& bytes) {
    return static_cast<bool>(WriteProcessMemory(handle, reinterpret_cast<LPVOID>(address), bytes.data(), bytes.size(), nullptr));
}

namespace {
    bool bytesMatch(std::vector<uint8_t>& a, std::vector<uint8_t>& b) {
        return a == b;
    }

    bool bytesMatch(std::vector<uint8_t>& a, std::vector<uint8_t>& b, const std::string& mask) {
        auto aIt = a.begin();
        auto bIt = b.begin();
        auto mIt = mask.begin();
        for (; mIt != mask.end(); ++aIt, ++bIt, ++mIt) {
            if (*mIt == 'x' && *aIt != *bIt)
                return false;
        }
        return true;
    }
}

uintptr_t Memory::findArrayOfBytes(std::vector<uint8_t>&& bytes, uint32_t protect, uintptr_t align) {
    auto target = std::move(bytes);

    if (aobCache.count(target)) {
        std::cout << "Found cached result" << std::endl;
        return aobCache[target];
    }

    for (auto& page : usablePages) {
        if (protect != 0 && page.protect != protect)
            continue;

        for (uintptr_t ptr = page.start; ptr < page.start + page.size - target.size(); ptr += align) {
            std::vector<uint8_t> read(bytes.size());
            ReadProcessMemory(handle, reinterpret_cast<LPCVOID>(ptr), read.data(), target.size(), nullptr);
            if (bytes == read) {
                aobCache.insert({target, ptr});
                return ptr;
            }
        }
    }
    return 0;
}

uintptr_t Memory::findArrayOfBytes(std::vector<uint8_t>&& bytes, const std::string& mask, uint32_t protect, uintptr_t align) {
    auto target = std::move(bytes);

    std::pair<std::vector<uint8_t>, std::string> pair = {target, std::string(mask)};
    if (aobMaskCache.count(pair)) {
        std::cout << "Found cached result" << std::endl;
        return aobMaskCache[pair];
    }

    for (auto& page : usablePages) {
        if (protect != 0 && page.protect != protect)
            continue;

        for (uintptr_t ptr = page.start; ptr < page.start + page.size - target.size(); ptr += align) {
            std::vector<uint8_t> read(target.size());
            ReadProcessMemory(handle, reinterpret_cast<LPCVOID>(ptr), read.data(), target.size(), nullptr);
            if (bytesMatch(target, read, mask)) {
                aobMaskCache.insert({pair, ptr});
                return ptr;
            }
        }
    }
    return 0;
}

uintptr_t Memory::allocate(size_t size, uint32_t protect, uintptr_t address) {
    return reinterpret_cast<uintptr_t>(VirtualAllocEx(handle, reinterpret_cast<LPVOID>(address), size, MEM_COMMIT, protect ? protect : PAGE_READWRITE));
}

void Memory::free(uintptr_t address) {
    VirtualFreeEx(handle, reinterpret_cast<LPVOID>(address), 0, MEM_RELEASE);
}

void Memory::findUsablePages() {
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    //std::cout << info.lpMinimumApplicationAddress << " - " << info.lpMaximumApplicationAddress << std::endl;
    for (uintptr_t address = reinterpret_cast<uintptr_t>(info.lpMinimumApplicationAddress);
         address <= reinterpret_cast<uintptr_t>(info.lpMaximumApplicationAddress);) {
        MEMORY_BASIC_INFORMATION page;
        VirtualQueryEx(handle, reinterpret_cast<LPCVOID>(address), &page, sizeof(page));

        if (page.State == MEM_COMMIT &&
            page.Type == MEM_PRIVATE &&
            page.Protect & (PAGE_READWRITE | PAGE_EXECUTE_READWRITE)) {
            usablePages.push_back({reinterpret_cast<uintptr_t>(page.BaseAddress), page.RegionSize, page.Protect});
        }

        address = reinterpret_cast<uintptr_t>(page.BaseAddress) + page.RegionSize;
    }
    //std::cout << "Found " << usablePages.size() << " writable pages." << std::endl;
}
