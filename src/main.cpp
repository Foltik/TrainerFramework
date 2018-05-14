#include "Console.h"
#include "Memory.h"
#include "Hotkeys.h"

#include <Windows.h>

int main() {
    Console::setTitle("StardewFisher");
    std::string processName = "StardewModdingAPI.exe";
    Memory mem(processName);
    Hotkeys hotkeys;

    hotkeys.add(VK_F6, "Increase Runspeed", HotkeyType::HELD, (const std::function<void(bool)>&)[&](bool enabled) {
        static uintptr_t addr = 0;
        static uintptr_t code = 0;

        if (enabled) {
            // Farmer::getRunSpeed
            addr = mem.findPattern({0x5D, 0x00, 0xD9, 0x45, 0x00, 0x8D, 0x65, 0xF4, 0x5B, 0x5E, 0x5F, 0x5D, 0xC3},
                                   "x?xx?xxxxxxxx", PAGE_EXECUTE_READWRITE, 16);

            // Allocate a page for the hook
            code = mem.allocate(0x1000, PAGE_EXECUTE_READWRITE);

            // Write the hook bytecode
            mem.writeData<uint8_t>(code, {0xDD, 0xD8, 0xD9, 0x05});
            mem.write<uintptr_t>(code + 0x4, code + 0x12);
            mem.writeData<uint8_t>(code + 0x8, {0x8D, 0x65, 0xF4, 0x5B, 0x5E, 0xE9});
            // Return to the original function
            mem.write<uintptr_t>(code + 0xE, (addr + 0xA) - (code + 0x12));
            mem.write<float>(code + 0x12, 15.0f);

            // Patch the function to jump to the hook
            mem.write<uint8_t>(addr + 0x5, 0xE9);
            mem.write<uintptr_t>(addr + 0x6, code - (addr + 0xA));
        } else {
            // Free the allocated page
            mem.free(code);

            // Restore the original function code
            mem.writeData<uint8_t>(addr + 0x5, {0x8D, 0x65, 0xF4, 0x5B, 0x5E});
        }
    });

    while (!hotkeys.shouldExit(VK_F9)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    return 0;
}