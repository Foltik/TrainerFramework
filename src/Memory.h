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

	HANDLE getHandle() const { return handle; };

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
		return static_cast<bool>(WriteProcessMemory(handle, reinterpret_cast<LPVOID>(address),
													&value, sizeof(T), nullptr));
	}

	template <typename T>
	bool writeData(uintptr_t address, const std::vector<T>& data) {
		return static_cast<bool>(WriteProcessMemory(handle, reinterpret_cast<LPVOID>(address),
													data.data(), sizeof(T) * data.size(), nullptr));
	}

	uintptr_t allocate(size_t size, uint32_t protect = 0, uintptr_t address = 0);
	void free(uintptr_t address);

    uintptr_t findPattern(const std::vector<uint8_t>& target, const std::string& mask = "", uint32_t protect = 0, uintptr_t align = 1);

private:
	std::map<std::pair<std::vector<uint8_t>, std::string>, uintptr_t> patternCache;

    void findUsablePages();
    std::vector<Page> usablePages;

	bool attached = false;
	HANDLE handle;
	DWORD pid;
};