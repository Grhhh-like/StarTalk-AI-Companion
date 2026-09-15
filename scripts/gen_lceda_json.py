# -*- coding: utf-8 -*-
"""生成星语StarTalk嘉立创EDA(标准版)工程JSON文件"""
import json
import uuid
import os

OUT = r"C:\Users\颜研\DoubaoWork\chats\2026-09-15\new-chat\StarTalk-AI-Companion\hardware\StarTalk_LCEDA.json"

def nid():
    return uuid.uuid4().hex[:24]

# 原理图元件: (类型, 名称, 位号, x, y, 旋转)
# 使用EasyEDA标准库器件编号(schLibId), 导入后可自动关联立创商城器件
parts = [
    # ESP32-S3 模组
    {"type":"ESP32S3WROOM1", "name":"ESP32-S3-WROOM-1", "ref":"U1", "x":200, "y":260, "rot":0,
     "schLibId":"C2896541"},
    # 麦克风
    {"type":"INMP441", "name":"INMP441", "ref":"U2", "x":60, "y":100, "rot":0,
     "schLibId":"C24112"},
    # 功放
    {"type":"MAX98357A", "name":"MAX98357A", "ref":"U3", "x":60, "y":420, "rot":0,
     "schLibId":"C2834888"},
    # 圆屏
    {"type":"GC9A01", "name":"GC9A01-1.28", "ref":"U4", "x":340, "y":100, "rot":0,
     "schLibId":"C2975552"},
    # 温湿度
    {"type":"SHT40", "name":"SHT40", "ref":"U5", "x":340, "y":420, "rot":0,
     "schLibId":"C5313433"},
    # 充电
    {"type":"TP4057", "name":"TP4057", "ref":"U6", "x":60, "y":680, "rot":0,
     "schLibId":"C20593"},
    # LDO
    {"type":"ME6211C33", "name":"ME6211-3.3V", "ref":"U7", "x":200, "y":680, "rot":0,
     "schLibId":"C82902"},
    # Type-C
    {"type":"USBC16P", "name":"USB-C", "ref":"USB1", "x":60, "y":540, "rot":0,
     "schLibId":"C2765186"},
    # 电池座
    {"type":"PH2-2P", "name":"BATTERY-PH2", "ref":"BT1", "x":340, "y":680, "rot":0,
     "schLibId":"C94556"},
    # 喇叭座
    {"type":"PH2-2P", "name":"SPEAKER-PH2", "ref":"LS1", "x":340, "y":540, "rot":0,
     "schLibId":"C94557"},
    # 按键
    {"type":"KEY6x6", "name":"KEY", "ref":"S1", "x":340, "y":560, "rot":0,
     "schLibId":"C92514"},
]

shapes = []
for p in parts:
    shapes.append({
        "type":"lib",
        "uuid": nid(),
        "name": p["name"],
        "device": p["ref"],
        "x": p["x"],
        "y": p["y"],
        "rot": p["rot"],
        "id": nid(),
        "schLibId": p["schLibId"],
        "gInfo": {"x": p["x"], "y": p["y"], "rot": p["rot"]},
    })

# 标题栏
shapes.append({
    "type":"text",
    "uuid": nid(),
    "x": 90, "y": 920, "rot": 0,
    "text": "星语 StarTalk 原理图",
    "font": {"size": 3.5, "name": "Arial"},
})

doc = {
    "head": {
        "docType": "3",
        "editorVersion": "6.5.32",
        "newId": uuid.uuid4().hex,
        "c_para": {"isFirst": "1"},
    },
    "canvasSettings": {
        "x": 10, "y": 10,
        "grid": 10,
        "w": 500, "h": 1000,
        "bgColor": "#FFFFFF",
        "gridColor": "#E0E0E0",
    },
    "shapes": shapes,
    "bom": {
        "v": 1,
        "items": [
            {"device": p["ref"], "name": p["name"], "quantity": 1,
             "schLibId": p["schLibId"]} for p in parts
        ],
    },
}

with open(OUT, "w", encoding="utf-8") as f:
    json.dump(doc, f, ensure_ascii=False, indent=1)

print("written:", OUT, os.path.getsize(OUT), "bytes")
# 验证JSON可解析
with open(OUT, encoding="utf-8") as f:
    json.load(f)
print("JSON valid")
