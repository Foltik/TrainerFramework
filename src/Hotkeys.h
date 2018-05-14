#pragma once

#include <functional>
#include <thread>
#include <future>
#include <queue>

using Callback = std::function<void(bool)>;
using VoidCallback = std::function<void()>;

enum class HotkeyType {
    ONESHOT,
    TOGGLE,
    HELD
};

struct Identifier {
    char key;
    std::string name;
};

struct Hotkey {
    Identifier id;
    Callback callback;
    bool state = false;
    bool toggle = false;
    std::thread runThread;
};

class Hotkeys {
public:
    Hotkeys();
    ~Hotkeys();

    void add(Identifier&& id, VoidCallback&& action);
    void addToggle(Identifier&& id, Callback&& action);
    void addHeld(Identifier&& id, Callback&& action);

    bool shouldExit(char key);

private:
    void pushCallback(std::function<void(bool)>& cb, bool arg);

    void tickKeys(std::future<void> exitSignal);
    void tickCalls(std::future<void> exitSignal);

    std::thread keypressThread;
    std::promise<void> kpExitSignal;

    std::mutex queueLock;
    std::queue<std::function<void()>> callQueue;
    std::thread callbackThread;
    std::promise<void> cbExitSignal;

    std::vector<std::pair<Hotkey, HotkeyType>> hotkeys;
};

