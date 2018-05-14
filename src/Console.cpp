#include "Console.h"

#include <Windows.h>

void Console::setTitle(const std::string& title) {
	SetConsoleTitleA(title.c_str());
}

void Console::setFlags(long flags) {
	HWND hwnd = GetConsoleWindow();
	long oldStyle = GetWindowLong(hwnd, GWL_STYLE);
	long newStyle = oldStyle | flags;
	SetWindowLong(hwnd, GWL_STYLE, newStyle);
}

void Console::setSize(short x, short y) {
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SMALL_RECT rect{ 0, 0, x, y };
	COORD size{ x, y };
	SetConsoleWindowInfo(handle, true, &rect);
	SetConsoleScreenBufferSize(handle, size);
}
