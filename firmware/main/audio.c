/**
 * @file audio.c
 * @brief I2S 音频: 双 I2S 总线(录音/播放)
 */
#include "audio.h"
#include "driver/i2s_std.h"
#include "driver/i2s.h"
#include "esp_log.h"

static const char *TAG = "audio";

#define I2S_RECV_PORT  I2S_NUM_0   /* INMP441  录音 */
#define I2S_PLAY_PORT  I2S_NUM_1   /* MAX98357A 播放 */

/* 引脚定义(与设计文档一致) */
#define PIN_RECV_SCK   GPIO_NUM_4
#define PIN_RECV_WS    GPIO_NUM_5
#define PIN_RECV_SD    GPIO_NUM_6

#define PIN_PLAY_BCLK  GPIO_NUM_15
#define PIN_PLAY_WS    GPIO_NUM_16
#define PIN_PLAY_DIN   GPIO_NUM_17

static i2s_chan_handle_t s_rx_chan;
static i2s_chan_handle_t s_tx_chan;

esp_err_t audio_init(void)
{
    /* ---- 录音通道: I2S0 RX, INMP441 ---- */
    i2s_chan_config_t rx_chan_cfg = {
        .id = I2S_RECV_PORT,
        .role = I2S_ROLE_MASTER,
        .dma_desc_num = 6,
        .dma_frame_num = 240,
        .auto_clear = true,
    };
    ESP_ERROR_CHECK(i2s_new_channel(&rx_chan_cfg, NULL, &s_rx_chan));

    i2s_std_config_t rx_std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(16000),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_32BIT, I2S_SLOT_MODE_MONO),
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,
            .bclk = PIN_RECV_SCK,
            .ws   = PIN_RECV_WS,
            .dout = I2S_GPIO_UNUSED,
            .din  = PIN_RECV_SD,
        },
    };
    rx_std_cfg.slot_cfg.slot_mask = I2S_STD_SLOT_LEFT;
    ESP_ERROR_CHECK(i2s_channel_init_std_mode(s_rx_chan, &rx_std_cfg));
    ESP_ERROR_CHECK(i2s_channel_enable(s_rx_chan));

    /* ---- 播放通道: I2S1 TX, MAX98357A ---- */
    i2s_chan_config_t tx_chan_cfg = {
        .id = I2S_PLAY_PORT,
        .role = I2S_ROLE_MASTER,
        .dma_desc_num = 6,
        .dma_frame_num = 240,
        .auto_clear = true,
    };
    ESP_ERROR_CHECK(i2s_new_channel(&tx_chan_cfg, &s_tx_chan, NULL));

    i2s_std_config_t tx_std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(16000),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,
            .bclk = PIN_PLAY_BCLK,
            .ws   = PIN_PLAY_WS,
            .dout = PIN_PLAY_DIN,
            .din  = I2S_GPIO_UNUSED,
        },
    };
    ESP_ERROR_CHECK(i2s_channel_init_std_mode(s_tx_chan, &tx_std_cfg));
    ESP_ERROR_CHECK(i2s_channel_enable(s_tx_chan));

    ESP_LOGI(TAG, "audio init ok (recv=I2S0, play=I2S1)");
    return ESP_OK;
}

esp_err_t audio_record_start(void) { return ESP_OK; }
esp_err_t audio_record_stop(void)  { return ESP_OK; }

esp_err_t audio_play(const uint8_t *data, size_t len, int sample_rate)
{
    size_t written = 0;
    /* 简单起见按当前采样率播放; 真实实现可切换 clk 速率 */
    esp_err_t err = i2s_channel_write(s_tx_chan, data, len, &written, portMAX_DELAY);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "i2s write failed: %s", esp_err_to_name(err));
    }
    return err;
}
