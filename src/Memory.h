#pragma once

#include <Windows.h>

#include <string>
#include <vector>
#include <map>

struct Page {
    uintptr_t start;
    size_t size;
    uint32_t protect;
};

class Memory {
public:
	Memory() = default;
	explicit Memory(const std::string& processName);
	~Memory();

	HANDLE getHandle() const;

	bool attach(const std::string& processName);
	void detach();

	template <typename T>
	T read(uintptr_t address) {
		T val;
		ReadProcessMemory(handle, static_cast<LPCVOID>(address), &val, sizeof(T), NULL);
		return val;
	}

	template <typename T>
	bool write(uintptr_t address, T value) {
		return static_cast<bool>(WriteProcessMemory(handle, reinterpret_cast<LPVOID>(address), &value, sizeof(T), NULL));
	}

	bool writeArrayOfBytes(uintptr_t address, std::vector<uint8_t>&& bytes);

	uintptr_t findArrayOfBytes(std::vector<uint8_t>&& bytes, uint32_t protect = 0, uintptr_t align = 1);
	uintptr_t findArrayOfBytes(std::vector<uint8_t>&& target, const std::string& mask, uint32_t protect = 0, uintptr_t align = 1);

	uintptr_t allocate(size_t size, uint32_t protect = 0, uintptr_t address = 0);
	void free(uintptr_t address);

private:
	std::map<std::vector<uint8_t>, uintptr_t> aobCache;
	std::map<std::pair<std::vector<uint8_t>, std::string>, uintptr_t> aobMaskCache;

    void findUsablePages();
    std::vector<Page> usablePages;

	bool attached = false;
	HANDLE handle;
	DWORD pid;
};