/**
 * @file asr.h
 * @brief ESP-SR 离线语音识别封装
 */
#pragma once
#include "esp_err.h"

esp_err_t asr_init(void);
esp_err_t asr_wait_wakeword(int timeout_ms);
esp_err_t asr_record_once(char *out_text, size_t out_len);
