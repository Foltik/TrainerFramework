#include "Hotkeys.h"

#include <Windows.h>
#include <iostream>

#define VK_PRESSED 0x8000

Hotkeys::Hotkeys() {
    keypressThread = std::thread(&Hotkeys::tickKeys, this, kpExitSignal.get_future());
    callbackThread = std::thread(&Hotkeys::tickCalls, this, cbExitSignal.get_future());
}

Hotkeys::~Hotkeys() {
    kpExitSignal.set_value();
    keypressThread.join();

    cbExitSignal.set_value();
    callbackThread.join();
}

void Hotkeys::add(Identifier&& id, VoidCallback&& action) {
    Hotkey hotkey{id, [=](bool b) { action(); }, 0, false};
    hotkeys.push_back({std::move(hotkey), HotkeyType::ONESHOT});
}

void Hotkeys::addToggle(Identifier&& id, Callback&& action) {
    Hotkey hotkey{id, action, false, false};
    hotkeys.push_back({std::move(hotkey), HotkeyType::TOGGLE});
}

void Hotkeys::addHeld(Identifier&& id, Callback&& action) {
    Hotkey hotkey{id, action, false, false};
    hotkeys.push_back({std::move(hotkey), HotkeyType::HELD});
}

void Hotkeys::pushCallback(std::function<void(bool)>& cb, bool arg) {
    queueLock.lock();
    callQueue.push(std::bind(cb, arg));
    queueLock.unlock();
}

void Hotkeys::tickKeys(std::future<void> exitSignal) {
    while (exitSignal.wait_for(std::chrono::milliseconds(0)) == std::future_status::timeout) {
        for (auto& pair : hotkeys) {
            auto& hotkey = pair.first;
            auto& type = pair.second;
            bool newState = static_cast<bool>(GetAsyncKeyState(hotkey.id.key) & VK_PRESSED);
            switch (type) {
                case HotkeyType::ONESHOT:
                    if (!hotkey.state && newState)
                        pushCallback(hotkey.callback, true);
                    break;
                case HotkeyType::TOGGLE:
                    if (!hotkey.state && newState) {
                        hotkey.toggle = !hotkey.toggle;
                        pushCallback(hotkey.callback, hotkey.toggle);
                    }
                    break;
                case HotkeyType::HELD:
                    if (hotkey.state != newState)
                        pushCallback(hotkey.callback, newState);
                    break;
            }
            hotkey.state = newState;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void Hotkeys::tickCalls(std::future<void> exitSignal) {
    while (exitSignal.wait_for(std::chrono::milliseconds(0)) == std::future_status::timeout) {
        std::function<void()> fn;
        queueLock.lock();
        if (!callQueue.empty()) {
            fn = callQueue.front();
            callQueue.pop();
        }
        queueLock.unlock();
        if (fn) fn();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

bool Hotkeys::shouldExit(char key) {
    return static_cast<bool>(GetAsyncKeyState(key) & VK_PRESSED);
}
