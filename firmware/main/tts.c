/**
 * @file tts.c
 * @brief TTS 语音合成: 请求 HTTP TTS 服务, 拉流后经 I2S 播放
 *
 * 支持火山引擎/阿里云/OpenAI 兼容 TTS 端点, 通过 menuconfig 配置:
 *   TTS_URL / TTS_TOKEN / TTS_VOICE
 */
#include "tts.h"
#include "audio.h"
#include "esp_log.h"
#include "esp_http_client.h"
#include "cJSON.h"

static const char *TAG = "tts";

#ifndef CONFIG_TTS_URL
#define CONFIG_TTS_URL "https://openspeech.bytedance.com/api/v1/tts"
#endif
#ifndef CONFIG_TTS_TOKEN
#define CONFIG_TTS_TOKEN "your-tts-token"
#endif
#ifndef CONFIG_TTS_VOICE
#define CONFIG_TTS_VOICE "zh_female_common"
#endif

static esp_err_t tts_http_handler(esp_http_client_event_t *evt)
{
    if (evt->event_id == HTTP_EVENT_ON_DATA) {
        /* 数据流直接送入 I2S 播放(假定服务端返回 16bit/16k PCM 或兼容格式) */
        audio_play((const uint8_t *)evt->data, evt->data_len, 16000);
    }
    return ESP_OK;
}

esp_err_t tts_say(const char *text)
{
    /* 构建 TTS 请求: 不同厂商字段不同, 此处以火山引擎风格为例 */
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "app", cJSON_AddStringToObject(root, "text", text) ? CONFIG_TTS_VOICE : "");
    char *payload = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);

    esp_http_client_config_t cfg = {
        .url = CONFIG_TTS_URL,
        .method = HTTP_METHOD_POST,
        .event_handler = tts_http_handler,
        .timeout_ms = 30000,
        .skip_cert_common_name_check = true,
    };
    esp_http_client_handle_t client = esp_http_client_init(&cfg);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_header(client, "Authorization", CONFIG_TTS_TOKEN);
    esp_http_client_set_post_field(client, payload, strlen(payload));

    esp_err_t err = esp_http_client_perform(client);
    esp_http_client_cleanup(client);
    free(payload);
    return err;
}

esp_err_t tts_play(const char *text) { return tts_say(text); }
