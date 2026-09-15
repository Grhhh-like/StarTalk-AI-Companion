/**
 * @file sensor.c
 * @brief SHT40 温湿度读取 (I2C)
 */
#include "sensor.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "driver/i2c_master.h"

static const char *TAG = "sensor";

#define SHT40_ADDR      0x44
#define CMD_MEAS_HIGH   0xFD
#define CMD_SOFT_RESET  0x94

#define PIN_I2C_SDA     GPIO_NUM_2
#define PIN_I2C_SCL     GPIO_NUM_3

static i2c_master_bus_handle_t s_bus;
static i2c_master_dev_handle_t s_dev;

esp_err_t sensor_init(void)
{
    i2c_master_bus_config_t bus_cfg = {
        .i2c_port = I2C_NUM_0,
        .sda_io_num = PIN_I2C_SDA,
        .scl_io_num = PIN_I2C_SCL,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_cfg, &s_bus));

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = SHT40_ADDR,
        .scl_speed_hz = 100000,
    };
    ESP_ERROR_CHECK(i2c_master_bus_add_device(s_bus, &dev_cfg, &s_dev));

    ESP_LOGI(TAG, "sht40 init ok");
    return ESP_OK;
}

esp_err_t sensor_read_temp_humi(float *temp_c, float *humi_rh)
{
    uint8_t cmd = CMD_MEAS_HIGH;
    uint8_t buf[6] = {0};

    ESP_ERROR_CHECK(i2c_master_transmit(s_dev, &cmd, 1, pdMS_TO_TICKS(100)));
    /* 高精度测量约 8.2ms */
    vTaskDelay(pdMS_TO_TICKS(10));
    esp_err_t err = i2c_master_receive(s_dev, buf, 6, pdMS_TO_TICKS(100));
    if (err != ESP_OK) {
        return err;
    }

    /* 温度 = -45 + 175 * raw / 65535 */
    uint16_t raw_t = (buf[0] << 8) | buf[1];
    /* 湿度 = 100 * raw / 65535 */
    uint16_t raw_h = (buf[3] << 8) | buf[4];

    *temp_c = -45.0f + 175.0f * (float)raw_t / 65535.0f;
    *humi_rh = 100.0f * (float)raw_h / 65535.0f;
    return ESP_OK;
}
