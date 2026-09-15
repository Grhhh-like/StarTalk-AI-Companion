idf_component_register(SRCS "main.c" "audio.c" "asr.c" "llm.c" "tts.c" "lcd.c" "sensor.c"
                    INCLUDE_DIRS "."
                    REQUIRES driver esp_wifi nvs_flash esp_netif esp_event
                              esp_https_client json esp-sr)

# 启用 C11 与数学库
target_compile_options(${COMPONENT_LIB} PRIVATE -std=c11 -Wno-unused-variable)
