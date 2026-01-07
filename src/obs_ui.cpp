#include <obs-module.h>
#include <thread>
#include <chrono>
#include <QMessageBox>

#include "plugin_state.h"
#include "bilibili_api.h"
#include "qr_dialog.h"

/* ================= 工具 ================= */

static void show_error(const std::string& msg)
{
    QMessageBox::critical(
        nullptr,
        "Bilibili Live",
        QString::fromStdString(msg)
    );
}

/* ================= Start ================= */

static bool on_start_clicked(obs_properties_t*, obs_property_t*, void*)
{
    if (g_state.runtime.busy.exchange(true))
        return false;

    std::thread([] {
        auto& cfg = g_state.config;

        /* 第一次 startLive */
        auto res = start_bilibili_live(
            cfg.room_id,
            cfg.area_id,
            cfg.title,
            cfg.cookie
        );

        /* 需要扫码 */
        if (res.need_qr) {
            QRDialog dlg(QString::fromStdString(res.qr_local_path));
            dlg.exec();

            bool confirmed = false;
            for (int i = 0; i < 30; ++i) { // 最多 60 秒
                std::string err;
                if (poll_qr_confirmed(cfg.cookie, err)) {
                    confirmed = true;
                    break;
                }
                std::this_thread::sleep_for(std::chrono::seconds(2));
            }

            if (!confirmed) {
                show_error("扫码超时或未确认");
                g_state.runtime.busy = false;
                return;
            }

            /* 再次 startLive */
            res = start_bilibili_live(
                cfg.room_id,
                cfg.area_id,
                cfg.title,
                cfg.cookie
            );
        }

        if (!res.success) {
            show_error(res.error);
            g_state.runtime.busy = false;
            return;
        }

        /* 设置 RTMP */
        obs_data_t* settings = obs_data_create();
        obs_data_set_string(settings, "server", res.rtmp_addr.c_str());
        obs_data_set_string(settings, "key", res.rtmp_key.c_str());

        obs_service_t* service =
            obs_service_create("rtmp_custom", "bilibili-rtmp",
                               settings, nullptr);

        obs_frontend_set_streaming_service(service);
        obs_service_release(service);
        obs_data_release(settings);

        obs_frontend_streaming_start();
        g_state.runtime.busy = false;
    }).detach();

    return false;
}

/* ================= Stop ================= */

static bool on_stop_clicked(obs_properties_t*, obs_property_t*, void*)
{
    if (g_state.runtime.busy.exchange(true))
        return false;

    std::thread([] {
        auto& cfg = g_state.config;

        auto res = stop_bilibili_live(
            cfg.room_id,
            cfg.cookie
        );

        if (!res.success) {
            show_error(res.error);
            g_state.runtime.busy = false;
            return;
        }

        obs_frontend_streaming_stop();
        g_state.runtime.busy = false;
    }).detach();

    return false;
}

/* ================= OBS Properties ================= */

obs_properties_t* obs_module_get_properties(void*)
{
    obs_properties_t* props = obs_properties_create();

    obs_properties_add_text(
        props, "room_id", "房间号 Room ID",
        OBS_TEXT_DEFAULT);

    obs_properties_add_int(
        props, "area_id", "分区 ID",
        0, 10000, 1);

    obs_properties_add_text(
        props, "title", "直播标题",
        OBS_TEXT_DEFAULT);

    obs_properties_add_text(
        props, "cookie", "Cookie（含 bili_jct）",
        OBS_TEXT_PASSWORD);

    obs_properties_add_button(
        props, "start_live", "🚀 开始直播",
        on_start_clicked);

    obs_properties_add_button(
        props, "stop_live", "🛑 停止直播",
        on_stop_clicked);

    return props;
}

/* ================= 配置更新 ================= */

void obs_module_update(obs_data_t* settings)
{
    auto& cfg = g_state.config;

    cfg.room_id = obs_data_get_string(settings, "room_id");
    cfg.area_id = (int)obs_data_get_int(settings, "area_id");
    cfg.title   = obs_data_get_string(settings, "title");
    cfg.cookie  = obs_data_get_string(settings, "cookie");
}
