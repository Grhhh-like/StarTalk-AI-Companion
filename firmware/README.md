# StarTalk AI Companion - ESP-IDF 固件

基于 **ESP-IDF v5.x** 的 ESP32-S3 固件工程。

## 功能模块

| 文件 | 功能 |
|---|---|
| `main.c` | 主循环、状态机、事件调度 |
| `asr.c` | ESP-SR 离线唤醒词识别（"你好小星"） |
| `llm.c` | OpenAI 兼容大模型 HTTP 客户端（流式） |
| `tts.c` | TTS 音频拉流与播放 |
| `audio.c` | I2S 录音/播放驱动 |
| `lcd.c` | GC9A01 圆形 LCD 表情显示 |
| `sensor.c` | SHT40 温湿度读取 |

## 快速开始

```bash
# 1. 设置目标芯片
idf.py set-target esp32s3

# 2. 配置 WiFi / API Key / TTS
idf.py menuconfig
#   → StarTalk Configuration:
#     WiFi SSID / Password
#     LLM API URL / Key / Model
#     TTS Provider / Token

# 3. 编译烧录
idf.py build
idf.py -p COMx flash monitor
```

## 依赖

- ESP-IDF v5.1+（含 `esp-sr` 语音组件，需单独安装到 components 目录）
- TTS 服务：火山引擎 / 阿里云 / OpenAI 兼容端点（任选其一，见 `main/config.h`）

## 状态机

```
 IDLE ──唤醒词──▶ LISTENING ──语音结束──▶ THINKING
   ▲                                        │
   │                                        ▼
 返回 ◀──语音播放── TALKING ◀──TTS合成─── RESPONSE
```

| 状态 | LCD 表情 | 说明 |
|---|---|---|
| IDLE | 眨眼 | 待机，低功耗 |
| LISTENING | 聆听 | 录音上传 |
| THINKING | 思考 | 等待 LLM 响应 |
| TALKING | 说话 | TTS 播放 |
