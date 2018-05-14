#include "Hotkeys.h"

#include <Windows.h>
#include <iostream>

#define VK_PRESSED 0x8000

Hotkey::Hotkey(char keyCode, const std::string& title, HotkeyType activationType, const std::function<void(bool)>& callback) {
    key = keyCode;
    name = title;
    type = activationType;
    cb = callback;
}

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

void Hotkeys::add(char keyCode, const std::string& title, HotkeyType type, const std::function<void(bool)>& action) {
    hotkeys.emplace_back(keyCode, title, type, action);
}

void Hotkeys::pushCallback(const std::function<void(bool)>& cb, bool arg) {
    queueLock.lock();
    callQueue.push(std::bind(cb, arg));
    queueLock.unlock();
}

void Hotkeys::tickKeys(std::future<void> exitSignal) {
    while (exitSignal.wait_for(std::chrono::milliseconds(0)) == std::future_status::timeout) {
        for (auto& hotkey : hotkeys) {
            bool newState = static_cast<bool>(GetAsyncKeyState(hotkey.key) & VK_PRESSED);
            switch (hotkey.type) {
                case HotkeyType::ONESHOT:
                    if (!hotkey.state && newState)
                        pushCallback(hotkey.cb, true);
                    break;
                case HotkeyType::TOGGLE:
                    if (!hotkey.state && newState) {
                        hotkey.toggle = !hotkey.toggle;
                        pushCallback(hotkey.cb, hotkey.toggle);
                    }
                    break;
                case HotkeyType::HELD:
                    if (hotkey.state != newState)
                        pushCallback(hotkey.cb, newState);
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
