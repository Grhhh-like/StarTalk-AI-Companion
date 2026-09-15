/**
 * @file tts.h
 * @brief TTS 语音合成与播放
 */
#pragma once
#include "esp_err.h"

esp_err_t tts_say(const char *text);
esp_err_t tts_play(const char *text);
