#pragma once

#include <functional>
#include <thread>
#include <future>

using Callback = std::function<void(bool)>;
using VoidCallback = std::function<void()>;

enum class HotkeyType {
    ONESHOT,
    TOGGLE,
    HELD
};

struct Identifier {
    char key;
    std::string_view name;
};

struct Hotkey {
    Identifier id;
    Callback callback;
    bool state = false;
    long lastChange = 0;
};

class Hotkeys {
public:
    Hotkeys();
    ~Hotkeys();

    void add(Identifier&& id, VoidCallback&& action);
    void addToggle(Identifier&& id, Callback&& action);
    void addHeld(Identifier&& id, Callback&& action);

private:
    void tick(std::future<void> exitSignal);

    std::thread hotkeyThread;
    std::promise<void> exitSignal;

    std::vector<std::pair<Hotkey, HotkeyType>> hotkeys;
};

