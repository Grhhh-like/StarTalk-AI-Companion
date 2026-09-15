/**
 * @file llm.c
 * @brief 大模型对话客户端: HTTPS 调用 OpenAI 兼容 Chat Completions API
 *
 * 通过 menuconfig 配置:
 *   LLM_API_URL / LLM_API_KEY / LLM_MODEL / SYSTEM_PROMPT
 */
#include "llm.h"
#include "esp_log.h"
#include "esp_http_client.h"
#include "cJSON.h"

static const char *TAG = "llm";

/* 配置项(实际应从 sdkconfig 读取, 此处给出默认值便于阅读) */
#ifndef CONFIG_LLM_API_URL
#define CONFIG_LLM_API_URL "https://api.openai.com/v1/chat/completions"
#endif
#ifndef CONFIG_LLM_API_KEY
#define CONFIG_LLM_API_KEY "sk-your-key"
#endif
#ifndef CONFIG_LLM_MODEL
#define CONFIG_LLM_MODEL "gpt-4o-mini"
#endif

#define MAX_RESPONSE 8192

typedef struct {
    char *buf;
    size_t len;
    size_t cap;
} resp_ctx_t;

static esp_err_t http_event_handler(esp_http_client_event_t *evt)
{
    resp_ctx_t *ctx = (resp_ctx_t *)evt->user_data;
    if (evt->event_id == HTTP_EVENT_ON_DATA) {
        size_t copy = evt->data_len;
        if (ctx->len + copy >= ctx->cap) {
            copy = ctx->cap - ctx->len - 1;
        }
        memcpy(ctx->buf + ctx->len, evt->data, copy);
        ctx->len += copy;
        ctx->buf[ctx->len] = '\0';
    }
    return ESP_OK;
}

esp_err_t llm_chat(const char *question, char *answer, size_t answer_len)
{
    cJSON *root = cJSON_CreateObject();
    cJSON *msgs = cJSON_AddArrayToObject(root, "messages");
    cJSON *sys = cJSON_CreateObject();
    cJSON_AddStringToObject(sys, "role", "system");
    cJSON_AddStringToObject(sys, "content", "你是星语桌面AI助手小星，回答简短友好。");
    cJSON_AddItemToArray(msgs, sys);
    cJSON *usr = cJSON_CreateObject();
    cJSON_AddStringToObject(usr, "role", "user");
    cJSON_AddStringToObject(usr, "content", question);
    cJSON_AddItemToArray(msgs, usr);
    cJSON_AddStringToObject(root, "model", CONFIG_LLM_MODEL);
    cJSON_AddNumberToObject(root, "max_tokens", 512);

    char *payload = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);

    resp_ctx_t ctx = {
        .buf = calloc(1, MAX_RESPONSE),
        .cap = MAX_RESPONSE,
    };
    if (!ctx.buf) { free(payload); return ESP_ERR_NO_MEM; }

    esp_http_client_config_t cfg = {
        .url = CONFIG_LLM_API_URL,
        .method = HTTP_METHOD_POST,
        .event_handler = http_event_handler,
        .user_data = &ctx,
        .timeout_ms = 30000,
        .skip_cert_common_name_check = true,
    };
    esp_http_client_handle_t client = esp_http_client_init(&cfg);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_header(client, "Authorization", CONFIG_LLM_API_KEY);
    esp_http_client_set_post_field(client, payload, strlen(payload));

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        /* 解析返回 JSON, 提取 choices[0].message.content */
        cJSON *resp = cJSON_Parse(ctx.buf);
        if (resp) {
            cJSON *choices = cJSON_GetObjectItem(resp, "choices");
            cJSON *first = cJSON_GetArrayItem(choices, 0);
            cJSON *msg = cJSON_GetObjectItem(first, "message");
            cJSON *content = cJSON_GetObjectItem(msg, "content");
            if (cJSON_IsString(content)) {
                snprintf(answer, answer_len, "%s", content->valuestring);
            }
            cJSON_Delete(resp);
        }
    } else {
        ESP_LOGE(TAG, "http error: %s", esp_err_to_name(err));
        snprintf(answer, answer_len, "抱歉，我暂时无法连接网络。");
    }

    esp_http_client_cleanup(client);
    free(ctx.buf);
    free(payload);
    return ESP_OK;
}
