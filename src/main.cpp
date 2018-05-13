#include "Console.h"
#include "Memory.h"
#include "Hotkeys.h"

#include <iostream>

int main() {
    Console::setTitle("StardewFisher");
    Memory mem;
    if (mem.attach("StardewModdingAPI.exe")) {
        std::cout << "Attached" << std::endl;
    }

    Hotkeys hotkeys;
    hotkeys.add({VK_F5, "Lolzor"}, []() {
        std::cout << "lul" << std::endl;
    });

    system("pause");
    return 0;
}