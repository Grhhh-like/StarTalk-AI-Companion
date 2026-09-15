/**
 * @file main.c
 * @brief 星语 StarTalk - 桌面级 AI 语音交互伴侣 主程序
 *
 * 状态机: IDLE -> LISTENING -> THINKING -> TALKING -> IDLE
 * 唤醒词: "你好小星" (ESP-SR 离线识别)
 * 对话:   OpenAI 兼容 LLM API (流式)
 * 输出:   TTS -> I2S 功放 -> 喇叭, 同步 LCD 表情
 */

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_netif.h"

#include "audio.h"
#include "asr.h"
#include "llm.h"
#include "tts.h"
#include "lcd.h"
#include "sensor.h"

static const char *TAG = "startalk";

/* ---------- 状态机 ---------- */
typedef enum {
    STATE_IDLE,
    STATE_LISTENING,
    STATE_THINKING,
    STATE_TALKING
} app_state_t;

static app_state_t g_state = STATE_IDLE;

static void set_state(app_state_t s)
{
    g_state = s;
    switch (s) {
    case STATE_IDLE:      lcd_show_face(LCD_FACE_IDLE);      break;
    case STATE_LISTENING: lcd_show_face(LCD_FACE_LISTENING); break;
    case STATE_THINKING:  lcd_show_face(LCD_FACE_THINKING);  break;
    case STATE_TALKING:   lcd_show_face(LCD_FACE_TALKING);   break;
    }
}

/* ---------- 应用主任务 ---------- */
static void app_main_task(void *arg)
{
    char question[4096];
    char answer[8192];

    while (1) {
        switch (g_state) {
        case STATE_IDLE: {
            /* 等待离线唤醒 */
            if (asr_wait_wakeword(5000) == ESP_OK) {
                ESP_LOGI(TAG, "wake word detected");
                tts_say("你好，我在。");
                set_state(STATE_LISTENING);
            } else {
                /* 空闲时周期读取温湿度, 保持传感器活跃 */
                float t, h;
                if (sensor_read_temp_humi(&t, &h) == ESP_OK) {
                    lcd_show_env(t, h);
                }
            }
            break;
        }
        case STATE_LISTENING: {
            /* 采集一句语音并转写(此处示意: 唤醒后录音, 交由云端ASR或本地命令词) */
            if (asr_record_once(question, sizeof(question)) == ESP_OK) {
                ESP_LOGI(TAG, "question: %s", question);
                set_state(STATE_THINKING);
            } else {
                set_state(STATE_IDLE);
            }
            break;
        }
        case STATE_THINKING: {
            /* 调用大模型 */
            if (llm_chat(question, answer, sizeof(answer)) == ESP_OK) {
                ESP_LOGI(TAG, "answer: %.100s...", answer);
                set_state(STATE_TALKING);
            } else {
                tts_say("抱歉，网络开小差了。");
                set_state(STATE_IDLE);
            }
            break;
        }
        case STATE_TALKING: {
            /* TTS 合成播放 */
            tts_play(answer);
            set_state(STATE_IDLE);
            break;
        }
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* 外设初始化 */
    audio_init();     /* I2S0 录音 + I2S1 播放 */
    lcd_init();       /* GC9A01 圆屏 */
    sensor_init();    /* SHT40 */
    asr_init();       /* ESP-SR 唤醒词 */

    /* WiFi 连接 */
    wifi_init_sta();  /* 见 audio.c 内封装或 wifi.c */

    lcd_show_face(LCD_FACE_IDLE);
    tts_say("初始化完成，你可以叫我小星。");

    xTaskCreate(app_main_task, "startalk_main", 8192, NULL, 5, NULL);
}
