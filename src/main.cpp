#include "Console.h"
#include "Hotkeys.h"
#include "Process.h"
#include "Memory.h"

#include <iostream>

#include <Windows.h>

int main() {
    Console::setTitle("StardewFisher");

    Process p("StardewModdingAPI.exe");
    if (!p) {
        std::cout << "Failed to attach to StardewModdingAPI.exe" << std::endl;
        std::cout << "Exiting..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        return 1;
    } else {
        std::cout << "Attached to StardewModdingAPI.exe" << std::endl;
    }


    Hotkeys hotkeys;
    hotkeys.add(VK_F5, "Speedhack", HotkeyType::TOGGLE, (const std::function<void(bool)>&)[&](bool enabled) {
        static uintptr_t addr = 0;
        static uintptr_t code = 0;

        if (enabled) {
            addr = Mem::findSignature<13>(p, {0x5D, 0xE8, 0xD9, 0x45, 0xE8, 0x8D, 0x65, 0xF4, 0x5B, 0x5E, 0x5F, 0x5D, 0xC3},
                                                       "", 16,
                                                       Mem::Protect::READ | Mem::Protect::WRITE |
                                                       Mem::Protect::EXECUTE);

            if (!addr)
                return;

            // Allocate a page for the hook
            code = Mem::alloc(p, 0x1000, Mem::Protect::READ | Mem::Protect::WRITE | Mem::Protect::EXECUTE);

            // Write the hook bytecode
            Mem::writeBytes<18>(p, code, {0xDD, 0xD8,                         // fstp  st(0)
                                          0xD9, 0x05, 0x00, 0x00, 0x00, 0x00, // fld   dword ptr [code + 0x12]
                                          0x8D, 0x65, 0xF4,                   // lea   esp, [ebp - 0C]
                                          0x5B,                               // pop   ebx
                                          0x5E,                               // pop   esi
                                          0xE9, 0x00, 0x00, 0x00, 0x00});     // jmp   addr + 0xA

            // Replace zeros with the actual pointers we need
            Mem::write<uintptr_t>(p, code + 0x4, code + 0x12);
            Mem::write<uintptr_t>(p, code + 0xE, (addr + 0xA) - (code + 0x12));

            // Speed
            Mem::write<float>(p, code + 0x12, 15.0f);

            // Patch the function to jump to the hook
            Mem::write(p, addr + 0x5, 0xE9);
            Mem::write(p, addr + 0x6, code - (addr + 0xA));
        } else {
            Mem::free(p, code);

            // Restore the original function code
            Mem::writeBytes<5>(p, addr + 0x5, {0x8D, 0x65, 0xF4,
                                               0x5B,
                                               0x5E});
        }
    });

    while (!hotkeys.shouldExit(VK_F9)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    return 0;
}