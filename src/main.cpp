#include "Console.h"
#include "Memory.h"
#include "Hotkeys.h"

#include <Windows.h>

int main() {
    Console::setTitle("StardewFisher");
    std::string processName = "StardewModdingAPI.exe";
    Memory mem(processName);

    Hotkeys hotkeys;

    hotkeys.addHeld({VK_F6, "Increase Runspeed"}, [&](bool enabled) {
        static uintptr_t addr;
        static uintptr_t code;

        if (enabled) {
            addr = mem.findArrayOfBytes({0x5D, 0x00, 0xD9, 0x45, 0x00, 0x8D, 0x65, 0xF4, 0x5B, 0x5E, 0x5F, 0x5D, 0xC3}, "x?xx??????xxx", PAGE_EXECUTE_READWRITE, 16);

            code = mem.allocate(0x1000, PAGE_EXECUTE_READWRITE);
            mem.writeArrayOfBytes(code, {0xDD, 0xD8, 0xD9, 0x05});
            mem.write<uintptr_t>(code + 0x4, code + 0x12);
            mem.writeArrayOfBytes(code + 0x8, {0x8D, 0x65, 0xF4, 0x5B, 0x5E, 0xE9});
            mem.write<uintptr_t>(code + 0xE, (addr + 0xA) - (code + 0x12));
            mem.write<float>(code + 0x12, 15.0f);

            mem.writeArrayOfBytes(addr + 0x5, {0xE9});
            mem.write<uintptr_t>(addr + 0x6, code - (addr + 0xA));


        } else {
            if (code) {
                mem.free(code);
                mem.writeArrayOfBytes(addr + 0x5, {0x8D, 0x65, 0xF4, 0x5B, 0x5E});
            }
        }
    });

    while(!hotkeys.shouldExit(VK_F9)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    return 0;
}