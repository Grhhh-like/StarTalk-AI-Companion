/**
 * @file sensor.h
 * @brief SHT40 温湿度传感器
 */
#pragma once
#include "esp_err.h"

esp_err_t sensor_init(void);
esp_err_t sensor_read_temp_humi(float *temp_c, float *humi_rh);
