/**
 * @file lcd.c
 * @brief GC9A01 1.28" 240x240 圆形 LCD 表情显示
 *
 * SPI 接口驱动(基于 ESP-IDF spi_master), 表情用内置位图数组。
 */
#include "lcd.h"
#include "esp_log.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

static const char *TAG = "lcd";

/* 引脚定义 */
#define PIN_LCD_SCK  GPIO_NUM_12
#define PIN_LCD_MOSI GPIO_NUM_11
#define PIN_LCD_DC   GPIO_NUM_10
#define PIN_LCD_CS   GPIO_NUM_9
#define PIN_LCD_RST  GPIO_NUM_8
#define PIN_LCD_BL   GPIO_NUM_7

static spi_device_handle_t s_spi;

/* 简易表情位图(8x8 示意, 真实工程使用 240x240 资源图) */
static const uint8_t face_idle[8][8] = {
    {0,0,1,1,1,1,0,0},
    {0,1,0,0,0,0,1,0},
    {1,0,0,1,1,0,0,1},
    {1,0,0,1,1,0,0,1},
    {1,0,0,0,0,0,0,1},
    {1,0,1,0,0,1,0,1},
    {0,1,0,0,0,0,1,0},
    {0,0,1,1,1,1,0,0},
};

static void lcd_write_cmd(uint8_t cmd)
{
    spi_transaction_t t = {
        .length = 8,
        .tx_buffer = &cmd,
    };
    spi_device_polling_transmit(s_spi, &t);
}

static void lcd_write_data(const uint8_t *data, size_t len)
{
    spi_transaction_t t = {
        .length = len * 8,
        .tx_buffer = data,
    };
    spi_device_polling_transmit(s_spi, &t);
}

esp_err_t lcd_init(void)
{
    spi_bus_config_t buscfg = {
        .mosi_io_num = PIN_LCD_MOSI,
        .miso_io_num = -1,
        .sclk_io_num = PIN_LCD_SCK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 240 * 240 * 2,
    };
    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 40 * 1000 * 1000,
        .mode = 0,
        .spics_io_num = PIN_LCD_CS,
        .queue_size = 7,
    };
    ESP_ERROR_CHECK(spi_bus_add_device(SPI2_HOST, &devcfg, &s_spi));

    gpio_set_direction(PIN_LCD_DC, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_LCD_RST, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_LCD_BL, GPIO_MODE_OUTPUT);

    /* 复位 */
    gpio_set_level(PIN_LCD_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(20));
    gpio_set_level(PIN_LCD_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(120));

    /* GC9A01 初始化序列(节选) */
    lcd_write_cmd(0xEF); lcd_write_cmd(0xEB); lcd_write_cmd(0xFE); lcd_write_cmd(0xEF);
    lcd_write_cmd(0x36); uint8_t mad = 0x00; lcd_write_data(&mad, 1);   /* MADCTL */
    lcd_write_cmd(0x3A); uint8_t pf = 0x05; lcd_write_data(&pf, 1);     /* 16bit */
    lcd_write_cmd(0x11); vTaskDelay(pdMS_TO_TICKS(120));                /* SLPOUT */
    lcd_write_cmd(0x29);                                               /* DISPON */

    gpio_set_level(PIN_LCD_BL, 1);
    ESP_LOGI(TAG, "lcd init ok");
    return ESP_OK;
}

esp_err_t lcd_show_face(lcd_face_t face)
{
    /* 实际工程: 显示对应表情资源; 此处示意将位图放大填满窗口 */
    ESP_LOGI(TAG, "face: %d", (int)face);
    return ESP_OK;
}

esp_err_t lcd_show_env(float temp, float humi)
{
    /* 实际工程: 在屏幕上绘制温湿度数值 */
    ESP_LOGI(TAG, "env: %.1fC %.1f%%RH", temp, humi);
    return ESP_OK;
}
