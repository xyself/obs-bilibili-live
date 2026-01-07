#include <iostream>

extern "C" void bilibili_plugin_initialize() {
    std::cout << "bilibili plugin initialize\n";
}

extern "C" void bilibili_plugin_shutdown() {
    std::cout << "bilibili plugin shutdown\n";
}
