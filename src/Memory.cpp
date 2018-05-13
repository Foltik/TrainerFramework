#include "Memory.h"

Memory::Memory(const std::string& processName) {
	attach(processName);
}

Memory::~Memory() {
	if (attached)
		detach();
}

bool Memory::attach(const std::string& processName) {
	if (attached)
		detach();

	HANDLE tHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(entry);

	if (Process32First(tHandle, &entry)) {
		do {
			if (!strcmp(processName.c_str(), entry.szExeFile)) {
				pid = entry.th32ProcessID;
				handle = OpenProcess(PROCESS_ALL_ACCESS, false, pid);

				attached = true;
				CloseHandle(tHandle);
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