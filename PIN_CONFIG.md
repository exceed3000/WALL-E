# WALL-E 引脚配置指南

## ESP32-S3 可用 GPIO 引脚

### ✅ 推荐使用的引脚
- **GPIO 1-21**: 大部分可用（除了 6-11）
- **GPIO 33, 35-37**: 仅输入或双向
- **GPIO 45-48**: 部分型号可用

### ❌ 避免使用的引脚
- **GPIO 6-11**: Flash/PSRAM 专用
- **GPIO 34-38**: 仅输入引脚（不能输出 PWM）
- **GPIO 44**: 通常连接板载 LED

## 默认引脚分配（可修改）

### I2S 音频接口
#### Audio Out (I2S1 - MAX98357A DAC/Amp)
- **I2S1_WS (LRC/WS)**: **GPIO 45**
- **I2S1_SCK (BCLK/SCK)**: **GPIO 47**
- **I2S1_SD (DIN/SD)**: **GPIO 48**

#### Audio In (I2S0 - Digital MEMS Mic)
- **I2S0_SD (Data Out)**: **GPIO 35**
- **I2S0_WS (LRCK)**: **GPIO 36**
- **I2S0_SCK (BCLK)**: **GPIO 37**
- **Config**: L/R 接 GND (左声道)

### 舵机控制 (7 个舵机 - MCPWM Group 0)
- **Servo 1**: **GPIO 18** - PWM Group 0
- **Servo 2**: **GPIO 8** - PWM Group 0
- **Servo 3**: **GPIO 9** - PWM Group 0
- **Servo 4**: **GPIO 10** - PWM Group 0
- **Servo 5**: **GPIO 46** - PWM Group 1
- **Servo 6**: **GPIO 4** - PWM Group 0
- **Servo 7**: **GPIO 5** - PWM Group 0

### 电机驱动板 (H 桥 - MCPWM)
- **IN1**: **GPIO 21** - Motor A 方向 1
- **IN2**: **GPIO 15** - Motor A 方向 2
- **IN3**: **GPIO 16** - Motor B 方向 1
- **IN4**: **GPIO 17** - Motor B 方向 2
- **PWM (可选)**: **-1** (未使用，可通过 menuconfig 启用)

## 修改引脚

运行以下命令配置引脚：

```bash
idf.py menuconfig
```

导航到：**WALL-E Configuration**

然后修改相应的引脚号。

## 注意事项

### 1. 舵机供电（重要！）
- **7 个舵机需要独立 5V 电源**
- 每个舵机工作电流约 100-500mA
- 堵转电流可达 1A+ 每个
- **总电流需求：3.5A - 7A**
- 建议使用 5V/5A 或更高功率的电源
- **必须共地**：电池负极 → ESP32-S3 GND

### 2. 电机驱动板供电
- 根据实际电机选择电源电压（通常 6-12V）
- TB6612FNG：最大 1.2A 连续电流
- L298N：最大 2A 峰值电流
- **必须共地**：电机电源负极 → ESP32-S3 GND

### 3. GPIO 引脚选择原则
- **PWM 输出**：选择支持 PWM 的引脚
- **避免输入专用引脚**：GPIO 34-39 不能输出
- **避免 Flash 引脚**：GPIO 6-11, 26-32
- **推荐 PWM 引脚组**：
  - Group 0: GPIO 0, 1, 2, 3, 4, 5, 15, 16, 17, 18
  - Group 1: GPIO 8, 9, 10, 46, 47, 48

### 4. I2S 音频引脚建议
- **Audio Out (I2S1)**: 使用 GPIO 45, 47, 48（相邻引脚，减少干扰）
- **Audio In (I2S0)**: 使用 GPIO 35, 36, 37（仅输入引脚，专用）
- 远离高频 PWM 引脚（避免干扰）

## MCP 工具调用示例

### 控制单个舵机
```json
{
  "index": 0,
  "angle": 90
}
```

### 控制电机
```json
{
  "direction": 1,
  "speed": 50
}
```

direction 参数：
- `1`: 前进
- `0`: 停止
- `-1`: 后退
