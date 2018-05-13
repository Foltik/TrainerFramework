#include "Hotkeys.h"

#include <Windows.h>

#define VK_PRESSED 0x8000

Hotkeys::Hotkeys() {
    hotkeyThread = {&Hotkeys::tick, this, exitSignal.get_future()};
}

Hotkeys::~Hotkeys() {

}

void Hotkeys::add(Identifier&& id, VoidCallback&& action) {
    Hotkey hotkey{id, [=](bool b) { action(); }, 0, false};
    hotkeys.push_back({std::move(hotkey), HotkeyType::ONESHOT});
}

void Hotkeys::addToggle(Identifier&& id, Callback&& action) {
    Hotkey hotkey{id, action, 0, false};
    hotkeys.push_back({std::move(hotkey), HotkeyType::TOGGLE});
}

void Hotkeys::addHeld(Identifier&& id, Callback&& action) {
    Hotkey hotkey{id, action, 0, false};
    hotkeys.push_back({std::move(hotkey), HotkeyType::HELD});
}

namespace {
    void handleOneshot(Hotkey& hotkey, bool newState) {
        if (hotkey.state) {
            hotkey.state = newState;
        }
    }

    void handleToggle(Hotkey& hotkey, bool newState) {

    }

    void handleHeld(Hotkey& hotkey, bool newState) {

    }
}

void Hotkeys::tick(std::future<void> exitSignal) {
    while (exitSignal.wait_for(std::chrono::milliseconds(0)) == std::future_status::timeout) {
        for (auto& pair : hotkeys) {
            auto&[hotkey, type] = pair;
            bool newState = static_cast<bool>(GetAsyncKeyState(hotkey.id.key) & VK_PRESSED);
            switch (type) {
                case HotkeyType::ONESHOT:
                    handleOneshot(hotkey, newState);
                    break;
                case HotkeyType::TOGGLE:
                    handleToggle(hotkey, newState);
                    break;
                case HotkeyType::HELD:
                    handleHeld(hotkey, newState);
                    break;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}
