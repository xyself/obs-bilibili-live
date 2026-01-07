cmake_minimum_required(VERSION 3.22)

project(obs_bilibili_live
    VERSION 1.1.0
    LANGUAGES C CXX
)

# =========================
# 基础配置
# =========================
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if (NOT WIN32)
    message(FATAL_ERROR "This plugin currently supports Windows only")
endif()

# =========================
# OBS 源码路径
# =========================
if (NOT OBS_SOURCE_DIR)
    message(FATAL_ERROR "OBS_SOURCE_DIR is not set")
endif()

set(OBS_LIBOBS_INCLUDE_DIR
    ${OBS_SOURCE_DIR}/libobs
)

set(OBS_FRONTEND_INCLUDE_DIR
    ${OBS_SOURCE_DIR}/UI/obs-frontend-api
)

# =========================
# 插件源码
# =========================
set(PLUGIN_SOURCES
    src/plugin.cpp
    src/obs_ui.cpp
    src/bilibili_api.cpp
    src/qr_dialog.cpp
)

set(PLUGIN_HEADERS
    src/plugin_state.h
    src/bilibili_api.h
    src/qr_dialog.h
)

add_library(obs-bilibili-live MODULE
    ${PLUGIN_SOURCES}
    ${PLUGIN_HEADERS}
)

# =========================
# include 路径
# =========================
target_include_directories(obs-bilibili-live PRIVATE
    ${OBS_LIBOBS_INCLUDE_DIR}
    ${OBS_FRONTEND_INCLUDE_DIR}

    # OBS 内置依赖头文件
    ${OBS_SOURCE_DIR}/deps/win64/include

    # 单头 json（如果你用了 nlohmann/json.hpp）
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# =========================
# Qt（使用 OBS 自带 Qt）
# =========================
set(Qt5_DIR
    ${OBS_SOURCE_DIR}/deps/win64/lib/cmake/Qt5
)

find_package(Qt5 REQUIRED COMPONENTS Widgets)

target_link_libraries(obs-bilibili-live PRIVATE
    Qt5::Widgets
)

# =========================
# OBS 核心库
# =========================
target_link_libraries(obs-bilibili-live PRIVATE
    obs
    obs-frontend-api
)

# =========================
# Windows 系统库
# =========================
target_link_libraries(obs-bilibili-live PRIVATE
    ws2_32
    crypt32
    bcrypt
    winhttp
)

# =========================
# 编译宏
# =========================
target_compile_definitions(obs-bilibili-live PRIVATE
    UNICODE
    _UNICODE
    OBS_FRONTEND_API_EXPORTS
)

# =========================
# DLL 输出设置
# =========================
set_target_properties(obs-bilibili-live PROPERTIES
    OUTPUT_NAME "bilibili-live"
    PREFIX ""
)

# =========================
# 安装（可选，本地调试用）
# =========================
install(TARGETS obs-bilibili-live
    RUNTIME DESTINATION obs-plugins/64bit
)
