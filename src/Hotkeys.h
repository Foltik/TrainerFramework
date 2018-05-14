#pragma once

#include <functional>
#include <thread>
#include <future>
#include <queue>

enum class HotkeyType {
    ONESHOT,
    TOGGLE,
    HELD
};

class Hotkey {
public:
    Hotkey(char keyCode, const std::string& title, HotkeyType activationType, const std::function<void(bool)>& callback);

    char key;
    std::string name;
    HotkeyType type;

    bool state = false;
    bool toggle = false;

    std::function<void(bool)> cb;
};

class Hotkeys {
public:
    Hotkeys();

    ~Hotkeys();

    void add(char keyCode, const std::string& title, HotkeyType type, const std::function<void(bool)>& action);

    bool shouldExit(char key);

private:
    void pushCallback(const std::function<void(bool)>& cb, bool arg);

    void tickKeys(std::future<void> exitSignal);

    void tickCalls(std::future<void> exitSignal);

    std::thread keypressThread;
    std::promise<void> kpExitSignal;

    std::mutex queueLock;
    std::queue<std::function<void()>> callQueue;
    std::thread callbackThread;
    std::promise<void> cbExitSignal;

    std::vector<Hotkey> hotkeys;
};

