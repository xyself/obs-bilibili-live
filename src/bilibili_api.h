#pragma once
#include <string>

/**
 * 获取 B 站直播间信息
 * @param room_id 直播间号
 * @return 原始 JSON 字符串，失败返回空字符串
 */
std::string bilibili_api_get_room_info(const std::string& room_id);
