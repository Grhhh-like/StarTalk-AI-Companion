# hardware/ — 硬件工程目录

本目录包含星语 StarTalk 的硬件设计文件。

## 文件说明

| 文件 | 说明 |
|---|---|
| `schematic.pdf` | 原理图（PDF 导出，见仓库 Releases 或 docs） |
| `StarTalk_LCEDA.json` | **嘉立创EDA（标准版）工程文件**，可在 oshwhub.com / 嘉立创EDA 直接打开编辑 |
| `BOM.csv` | 物料清单（含立创商城参考编号） |
| `PCB_top.png` | PCB 3D 预览（顶面） |

## 如何使用嘉立创EDA工程

### 方式一：在线打开（推荐）

1. 打开 https://oshwhub.com 并登录；
2. 点击「创建工程」→「导入工程」；
3. 上传 `StarTalk_LCEDA.json`；
4. 导入后即可查看/编辑原理图与 PCB。

### 方式二：本地嘉立创EDA专业版

1. 打开嘉立创EDA专业版；
2. 「文件」→「导入」→「EasyEDA 文件」；
3. 选择 `StarTalk_LCEDA.json`。

## PCB 关键参数

| 参数 | 值 |
|---|---|
| 层数 | 2 层 |
| 尺寸 | 45mm × 45mm |
| 板厚 | 1.6mm |
| 最小线宽/间距 | 6mil / 6mil |
| 表面处理 | 沉金或喷锡 |

## 打样建议

- 使用嘉立创每月 2 次免费打样（10cm×10cm 内）；
- 下单时选择「确认生产稿」前先跑一遍 DRC（间距、未连接网络检查）；
- 钢网可选：0402 器件较多时建议开钢网，提高焊接良率。
