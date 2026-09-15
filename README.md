# 星语 StarTalk · 桌面级 AI 语音交互伴侣

![license](https://img.shields.io/badge/license-MIT-green)
![platform](https://img.shields.io/badge/platform-ESP32--S3-blue)
![AI](https://img.shields.io/badge/AI-LLM%20%2B%20ASR%20%2B%20TTS-orange)

> 一个开源的桌面 AI 语音助手：离线语音唤醒，接入大模型对话，圆形彩屏表情反馈，环境温湿度感知。全链路开源，复刻成本约 **100–150 元**。

---

## 项目简介

**星语 StarTalk** 是一个基于 **ESP32-S3** 的桌面级 AI 语音交互伴侣。

- 🔥 **离线语音唤醒**：基于乐鑫 ESP-SR 语音识别框架，本地识别唤醒词"你好小星"，不上传音频；
- 🧠 **大模型对话**：通过 WiFi 接入任意 OpenAI 兼容的 LLM API（可配置），支持流式对话；
- 🔊 **本地语音合成**：对接 TTS 服务（火山引擎/阿里云/OpenAI 兼容端点均可配置），AI 开口说话；
- 😊 **表情屏**：1.28 英寸 GC9A01 圆形 LCD，根据对话状态实时切换表情动画；
- 🌡️ **环境感知**：板载 SHT40 温湿度传感器，可语音查询"现在温度多少"；
- 🔋 **电池供电**：3.7V 锂电池 + TP4057 充电管理，充满续航数小时，Type-C 充电；
- 🎨 **嘉立创EDA 全开源**：原理图、PCB、固件、外壳全部开源，嘉立创每月两次免费打样可复刻。

## 为什么做这个项目

2025 年星火计划获奖作品中，"自动驾驶迷你智能车"和"四模交互机械键盘"代表了**极致集成 + 人机交互新范式**两个方向；2026 年，边缘 AI 与语音交互正成为开源硬件最热赛道（ESP32 生态、xiaozhi-esp32 等项目的爆火）。**星语 StarTalk** 把大模型能力搬上桌面，用一颗 10 元级主控实现"能听、能说、有表情、懂环境"的完整 AI 交互闭环，入门门槛低、可玩性高、扩展空间大，适合初级赛道参赛。

## 技术方案

```
┌─────────────────────────────────────────────────────┐
│                    星语 StarTalk                      │
│                                                     │
│  ┌──────────┐  I2S   ┌──────────────┐              │
│  │ INMP441  │──────▶│              │   SPI        │
│  │ 数字麦克风 │       │   ESP32-S3    │──────▶ GC9A01  │
│  └──────────┘        │   (WROOM-1)  │   圆形LCD(表情)│
│  ┌──────────┐  I2S   │              │              │
│  │ MAX98357A│◀──────│  ESP-SR唤醒   │   I2C        │
│  │ 功放+喇叭  │       │  LLM客户端    │──────▶ SHT40   │
│  └──────────┘        │  TTS播放      │   温湿度      │
│                      └──────────────┘              │
│  ┌──────────┐      │  ┌──────────┐                │
│  │ TP4057   │      │  │ ME6211   │                │
│  │ 锂电池充电 │──────┴─▶│ 3.3V LDO  │                │
│  └──────────┘ 3.7V   │  └──────────┘                │
│   Type-C 输入         └─────────────────────────────┘
```

| 模块 | 器件 | 接口 | 说明 |
|---|---|---|---|
| 主控 | ESP32-S3-WROOM-1 (N8R8) | — | 双核 240MHz，WiFi/BLE，AI 加速指令 |
| 麦克风 | INMP441 | I2S0 | MEMS 数字麦克风，24bit，离线唤醒输入 |
| 功放 | MAX98357A | I2S1 | 3W D 类功放，驱动 8Ω/3W 喇叭 |
| 屏幕 | GC9A01 1.28" 圆屏 | SPI | 240×240 表情/状态显示 |
| 温湿度 | SHT40 | I2C | 高精度温湿度传感器 |
| 充电 | TP4057 | — | 500mA 线性锂电充电 |
| 稳压 | ME6211 | — | 3.3V/500mA LDO |
| 电池 | 3.7V 锂聚合物 800–1200mAh | — | 续航数小时 |

## 快速开始

### 硬件准备

1. 按 `hardware/StarTalk_LCEDA.json` 导入嘉立创EDA，下单打样 PCB（嘉立创每月 2 次免费打样可直接覆盖）；
2. 按 `hardware/BOM.csv` 在立创商城采购元器件；
3. 参考 `docs/assembly.md` 焊接组装。

### 固件烧录

```bash
# 需要 ESP-IDF v5.x 环境
cd firmware
idf.py set-target esp32s3
idf.py menuconfig   # 填入 WiFi 账号、LLM API Key、TTS 配置
idf.py build
idf.py -p COMx flash monitor
```

### 配置大模型

编辑 `firmware/main/config.h` 或通过 menuconfig 填入：

```c
#define WIFI_SSID      "your_wifi"
#define WIFI_PASS      "your_password"
#define LLM_API_URL    "https://api.openai.com/v1/chat/completions"
#define LLM_API_KEY    "sk-xxxx"
#define LLM_MODEL      "gpt-4o-mini"
#define WAKE_WORD      "你好小星"
```

## 使用演示

1. 上电后等待语音提示"初始化完成"；
2. 说 **"你好小星"** 唤醒设备，屏幕显示"听"表情；
3. 说出问题，如 **"现在室温多少度？"** 或 **"给我讲个笑话"**；
4. AI 回答通过喇叭播出，屏幕同步显示"说"表情动画；
5. 空闲 10 秒自动休眠待唤醒，省电。

## 目录结构

```
StarTalk-AI-Companion/
├── README.md                 # 项目说明（本文件）
├── LICENSE                   # MIT 开源协议
├── hardware/                 # 硬件工程
│   ├── StarTalk_LCEDA.json   # 嘉立创EDA工程文件（可导入编辑）
│   ├── BOM.csv               # 物料清单（含立创商城编号）
│   └── README.md             # 硬件目录与打样说明
├── firmware/                 # 固件（ESP-IDF）
│   ├── main/
│   │   ├── main.c            # 主循环与状态机
│   │   ├── audio.c/.h        # I2S 录音/播放
│   │   ├── asr.c/.h          # ESP-SR 离线唤醒
│   │   ├── llm.c/.h          # 大模型 HTTP 客户端
│   │   ├── tts.c/.h          # TTS 拉流播放
│   │   ├── lcd.c/.h          # 表情屏驱动
│   │   ├── sensor.c/.h       # SHT40 温湿度
│   │   └── wifi.c/.h         # WiFi 连接
│   ├── sdkconfig.defaults    # 默认配置（WiFi/API/TTS）
│   └── CMakeLists.txt
├── docs/
│   ├── design.md             # 设计文档与选型说明
│   └── assembly.md           # 焊接组装与调试指南
├── assets/
│   ├── schematic_block_diagram.svg  # 原理框图
│   └── pcb_preview.png              # PCB 布局示意
└── scripts/
    └── gen_lceda_json.py     # 嘉立创EDA工程生成脚本
```

## 开源协议

本项目基于 **MIT License** 开源，欢迎 fork、复刻、二创。商用请遵守各 API 服务商条款。

## 致谢

- 乐鑫 ESP-IDF / ESP-SR 语音识别框架
- xiaozhi-esp32 项目的交互设计启发
- 立创开源硬件平台 · 星火计划

---

**StarTalk 星语** — 让每个桌面都拥有一个会聊天的 AI。
