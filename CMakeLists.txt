cmake_minimum_required(VERSION 3.5)
project(bilibili-live-plugin VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(SRC_FILES
    src/plugin.cpp
    src/obs_ui.cpp
    src/bilibili_api.cpp
)

add_library(bilibili_live_plugin SHARED ${SRC_FILES})
target_include_directories(bilibili_live_plugin PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

if (WIN32)
    set_target_properties(bilibili_live_plugin PROPERTIES PREFIX "" SUFFIX ".dll")
endif()
