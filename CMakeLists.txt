cmake_minimum_required(VERSION 3.25)
project(obs-bilibili-live VERSION 1.0.8 LANGUAGES CXX)

# ========= 基本配置 =========
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# ========= OBS 路径校验 =========
if(NOT DEFINED OBS_SOURCE_DIR)
    message(FATAL_ERROR "OBS_SOURCE_DIR is not set")
endif()

set(OBS_BUILD_DIR ${OBS_SOURCE_DIR}/build)
if(NOT EXISTS ${OBS_BUILD_DIR})
    message(FATAL_ERROR "OBS build directory not found: ${OBS_BUILD_DIR}")
endif()

# ========= 查找 OBS =========
list(APPEND CMAKE_PREFIX_PATH ${OBS_BUILD_DIR})
find_package(libobs REQUIRED)

# ========= 依赖 =========
find_package(CURL REQUIRED)

# ========= Qt =========
set(QT_VERSION_MAJOR 6)
find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()

# ========= 插件 =========
add_library(obs-bilibili-live MODULE
    src/plugin.cpp
    src/obs_ui.cpp
    src/bilibili_api.cpp
    src/qr_dialog.cpp
)

target_include_directories(obs-bilibili-live PRIVATE
    src
    src/third_party
    ${OBS_SOURCE_DIR}/libobs
)

target_link_libraries(obs-bilibili-live
    libobs
    Qt6::Widgets
    CURL::libcurl
)

set_target_properties(obs-bilibili-live PROPERTIES
    PREFIX ""
    OUTPUT_NAME "bilibili-live"
)

