/**
 * @file audio.h
 * @brief I2S 音频驱动: INMP441 录音 + MAX98357A 播放
 */
#pragma once
#include "esp_err.h"

esp_err_t audio_init(void);
esp_err_t audio_record_start(void);
esp_err_t audio_record_stop(void);
esp_err_t audio_play(const uint8_t *data, size_t len, int sample_rate);
