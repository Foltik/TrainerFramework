#include "Console.h"
#include "Hotkeys.h"
#include "mem/Process.h"
#include "mem/Memory.h"
#include "script/Script.h"
#include "script/Lexer.h"

#include <iostream>

#include <Windows.h>

int main() {
    auto lol = Script(R"(
        {addr+0x5}:
            jmp     code
        $return:

        {code}:
            fpop
            fld     [speed]
            .db      0x8D, 0x65, 0xF4
            pop     ebx
            pop     esi
            jmp     return
        $speed:
            .dfl     0f15.0
    )", {});

    return 0;

    Console::setTitle("TrainerFramework");

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
            if (!addr)
                addr = Mem::findSignature<13>(p,
                                              {0x5D, 0xE8, 0xD9, 0x45, 0xE8, 0x8D, 0x65, 0xF4, 0x5B, 0x5E, 0x5F, 0x5D,
                                               0xC3},
                                              "x?xx??????xxx", 8,
                                              Mem::Protect::READ | Mem::Protect::WRITE | Mem::Protect::EXECUTE);

            code = Mem::alloc(p, 0x1000);

            Script(R"(
                {addr+0x5}:
                    jmp     code
                return:

                {code}:
                    fpop
                    fld     [speed]
                    db      0x8D, 0x65, 0xF4
                    pop     ebx
                    pop     esi
                    jmp     return
                speed:
                    dfl     15.0

            )", {{"addr", addr},
                 {"code", code}}).execute(p);

        } else {
            Script(R"(
                {addr+0x5}:
                    db      0x8D, 0x65, 0xF4, 0x5B, 0x5E

            )", {{"addr", addr}}).execute(p);

            Mem::free(p, code);
        }
    });

    while (!hotkeys.shouldExit(VK_ESCAPE)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    return 0;
}