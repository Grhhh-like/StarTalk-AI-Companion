/**
 * @file asr.c
 * @brief 基于乐鑫 ESP-SR 的离线唤醒词 + 命令词识别
 *
 * 依赖组件: esp-sr (需在 components 目录放置 esp-sr 组件)
 * 唤醒词: "你好小星" (可自定义训练模型)
 */
#include "asr.h"
#include "esp_log.h"
#include "esp_sr.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "asr";

static model_iface_data_t *s_model_data = NULL;
static char s_wake_word[32] = "你好小星";

/* 唤醒词识别结果回调 */
static void wakeword_cb(sr_wakeup_ret_t wakeup_ret, void *userdata)
{
    ESP_LOGI(TAG, "wakeup: %s", wakeup_ret.word);
}

esp_err_t asr_init(void)
{
    /* 加载 WakeNet 唤醒词模型(通过 menuconfig 选择 wakenet 型号) */
    srmodel_list_t *model_list = esp_srmodel_init("model");
    if (!model_list) {
        ESP_LOGE(TAG, "failed to init esp-sr model");
        return ESP_FAIL;
    }
    s_model_data = esp_srmodel_filter(model_list, ESP_WN_PREFIX, ESP_WN_MODEL_ID_CN, NULL);
    if (!s_model_data) {
        ESP_LOGE(TAG, "wakenet model not found");
        return ESP_FAIL;
    }

    /* 创建唤醒识别句柄(此处示意 API, 以 ESP-SR 版本为准) */
    ESP_LOGI(TAG, "asr init ok, wake word: %s", s_wake_word);
    return ESP_OK;
}

esp_err_t asr_wait_wakeword(int timeout_ms)
{
    /* 真实实现: 从 I2S0 取音频送入 WakeNet 推理, 命中回调置标志 */
    /* 此处为占位实现: 超时返回 */
    vTaskDelay(pdMS_TO_TICKS(timeout_ms));
    return ESP_ERR_TIMEOUT;
}

esp_err_t asr_record_once(char *out_text, size_t out_len)
{
    /* 真实实现: 唤醒后采集一句音频, 可选本地命令词识别或上传云端 ASR */
    /* 此处为占位实现 */
    if (out_len > 0) {
        snprintf(out_text, out_len, "帮我查一下当前温度");
    }
    return ESP_OK;
}
