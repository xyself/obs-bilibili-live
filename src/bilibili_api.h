#pragma once
#include <string>

struct StartLiveResult {
    bool success = false;
    bool need_qr = false;

    std::string qr_url;
    std::string qr_local_path;

    std::string rtmp_addr;
    std::string rtmp_key;

    std::string error;
};

struct SimpleResult {
    bool success = false;
    std::string error;
};

/* 原有接口 */
StartLiveResult start_bilibili_live(
    const std::string& room_id,
    int area_id,
    const std::string& title,
    const std::string& cookie
);

SimpleResult stop_bilibili_live(
    const std::string& room_id,
    const std::string& cookie
);

SimpleResult update_bilibili_title(
    const std::string& room_id,
    const std::string& title,
    const std::string& cookie
);

/* 🔥 新增：扫码状态轮询 */
bool poll_qr_confirmed(
    const std::string& cookie,
    std::string& error
);
