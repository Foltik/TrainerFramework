#pragma once

#include <string>
#include <Windows.h>
#include <TlHelp32.h>

class Memory {
public:
	Memory() = default;
	explicit Memory(const std::string& processName);
	~Memory();

	bool attach(const std::string& processName);
	void detach();

	template <typename T>
	T read(LPVOID address) const {
		T val;
		ReadProcessMemory(handle, address, &val, sizeof(T), NULL);
		return val;
	}

	template <typename T>
	T write(LPVOID address, T value) const {
		return WriteProcessMemory(handle, address, &value, sizeof(T), NULL);
	}

private:
	bool attached = false;
	HANDLE handle;
	DWORD pid;
};