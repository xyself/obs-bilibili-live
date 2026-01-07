#pragma once
#include <string>
#include <atomic>
#include <mutex>

struct PluginConfig {
    std::string room_id;
    int area_id = 0;
    std::string title;
    std::string cookie;
};

struct RuntimeState {
    std::atomic_bool busy{false};
    std::atomic_bool living{false};
    std::string last_error;
};

struct PluginState {
    PluginConfig config;
    RuntimeState runtime;
    std::mutex lock;
};

extern PluginState g_state;
