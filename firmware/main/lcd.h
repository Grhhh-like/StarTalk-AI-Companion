/**
 * @file lcd.h
 * @brief GC9A01 圆形 LCD 表情屏驱动
 */
#pragma once
#include "esp_err.h"

typedef enum {
    LCD_FACE_IDLE,       /* 眨眼待机 */
    LCD_FACE_LISTENING,  /* 聆听 */
    LCD_FACE_THINKING,   /* 思考 */
    LCD_FACE_TALKING,    /* 说话 */
    LCD_FACE_MAX
} lcd_face_t;

esp_err_t lcd_init(void);
esp_err_t lcd_show_face(lcd_face_t face);
esp_err_t lcd_show_env(float temp, float humi);
