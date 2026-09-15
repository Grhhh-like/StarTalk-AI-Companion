/**
 * @file llm.h
 * @brief 大模型 HTTP 客户端 (OpenAI 兼容 API)
 */
#pragma once
#include "esp_err.h"

esp_err_t llm_chat(const char *question, char *answer, size_t answer_len);
