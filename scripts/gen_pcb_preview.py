# -*- coding: utf-8 -*-
"""生成StarTalk PCB 3D预览示意(顶面) PNG, 用于README展示"""
from PIL import Image, ImageDraw, ImageFont

W, H = 800, 800
img = Image.new("RGB", (W, H), "#1e3a5f")
d = ImageDraw.Draw(img)

# 板子轮廓 45x45mm -> 居中
board = [ (100,100), (700,100), (700,700), (100,700) ]
d.rounded_rectangle([100,100,700,700], radius=30, fill="#0f7a4d", outline="#0a5c3a", width=6)

def chip(x1,y1,x2,y2,color,label,font):
    d.rounded_rectangle([x1,y1,x2,y2], radius=8, fill=color, outline="black", width=2)
    bbox = d.textbbox((0,0), label, font=font)
    d.text(((x1+x2)/2-(bbox[2]-bbox[0])/2, (y1+y2)/2-(bbox[3]-bbox[1])/2), label, fill="white", font=font)

try:
    f_big = ImageFont.truetype("msyh.ttc", 22)
    f_mid = ImageFont.truetype("msyh.ttc", 16)
except Exception:
    f_big = ImageFont.load_default()
    f_mid = ImageFont.load_default()

# ESP32-S3 模组(中心)
chip(310, 330, 490, 430, "#1f4e8c", "ESP32-S3", f_mid)
# 天线区域
d.rectangle([310, 430, 490, 470], fill="#0a5c3a", outline="black", width=2)
d.text((360, 440), "ANTENNA", fill="white", font=f_mid)

# 麦克风
chip(150, 170, 260, 240, "#0e7a5f", "INMP441", f_mid)
# 功放+喇叭区
chip(150, 520, 280, 610, "#b3561a", "MAX98357A", f_mid)
d.ellipse([180, 620, 260, 700], fill="#8b4513", outline="black", width=2)
d.text((195, 665), "SPK", fill="white", font=f_mid)

# LCD屏(圆)
d.ellipse([470, 150, 630, 310], fill="#6a3ab2", outline="black", width=2)
d.text((515, 220), "GC9A01", fill="white", font=f_mid)

# 传感器
chip(560, 520, 660, 590, "#a31e3c", "SHT40", f_mid)
# 电源
chip(150, 360, 260, 430, "#ffd76a", "TP4057", f_mid)
chip(300, 540, 390, 610, "#e8c9f0", "LDO", f_mid)
# USB-C
d.rounded_rectangle([300, 690, 500, 730], radius=6, fill="#444", outline="black", width=2)
d.text((360, 700), "USB-C", fill="white", font=f_mid)
# 电池座
chip(560, 360, 660, 430, "#c9d9f0", "BAT", f_mid)

# 标题
d.text((200, 40), "StarTalk AI Companion · PCB 布局示意 (45x45mm 2层)", fill="white", font=f_big)
d.text((180, 760), "嘉立创EDA 全开源 · 免费打样可复刻", fill="#9fc3e8", font=f_mid)

OUT = r"C:\Users\颜研\DoubaoWork\chats\2026-09-15\new-chat\StarTalk-AI-Companion\assets\pcb_preview.png"
img.save(OUT)
print("saved:", OUT)
