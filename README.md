# 🏠 ESP32 Smart Home Controller

[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)

ESP32-based smart home project featuring:  
✅ Web UI to control Lamp, Fan, and Device  
✅ PIR auto-lamp with manual override  
✅ DHT22 auto-fan (temperature-based)  
✅ IR remote control  
✅ OLED (SSD1306) live status display

---

## 🎥 Demo
[![Watch the demo](https://img.youtube.com/vi/7I54fgAhxs4/0.jpg)](https://youtu.be/7I54fgAhxs4)

---

## ✨ Features
- Web server with buttons for **Lamp / Fan / Device** + **Toggle Fan Mode (Auto/Manual)**
- **Auto fan** when `TempF > threshold` (tunable)
- **PIR** motion turns Lamp ON, auto-OFF after timeout (manual override respected)
- **IR remote**: buttons map to Lamp/Fan/Device + Fan Auto toggle
- **OLED** shows IP, temperature, and current states

---

## 🧰 Hardware
- ESP32 dev board  
- 3-channel relay module  
- PIR motion sensor  
- DHT22 temperature/humidity sensor  
- SSD1306 OLED 128×64 (I²C, addr `0x3C`)  
- IR receiver (e.g., VS1838B)  
- Shared GND between all modules and ESP32

---

## 🪛 Wiring
> Pins are taken directly from `src/main.ino`.

| Module / Signal        | ESP32 Pin | Notes                                                  |
|------------------------|-----------|--------------------------------------------------------|
| **Relay 1 (Lamp)**     | GPIO23    | To relay IN1                                          |
| **Relay 2 (Fan)**      | GPIO22    | To relay IN2                                          |
| **Relay 3 (Device)**   | GPIO21    | To relay IN3                                          |
| **PIR OUT**            | GPIO19    | VCC→5V/3V3, GND→GND                                   |
| **DHT22 DATA**         | GPIO18    | VCC→3V3, GND→GND (10k pull-up to 3V3 if module lacks) |
| **IR Receiver OUT**    | GPIO17    | VCC→3V3 (or 5V per module), GND→GND                   |
| **OLED SDA**           | GPIO27    | I²C via `Wire.begin(27, 26)`                           |
| **OLED SCL**           | GPIO26    | I²C via `Wire.begin(27, 26)`                           |
| **Relay VCC/GND**      | 5V + GND  | Many relay boards need 5V coil; share **GND**         |

**Notes**
- **Common ground is mandatory.**
- If your relay board is **active-LOW**, invert the `digitalWrite()` logic.
- PIR modules vary; use the onboard sensitivity/time pots as needed.
- For safety with **mains AC**, keep high-voltage wiring isolated and follow proper practices.

---

## 🚀 Getting Started
1. **Secrets**: copy `include/config.example.h` → `include/config.h`, then set:
   ```cpp
   #define WIFI_SSID "YourWiFiName"
   #define WIFI_PASS "YourWiFiPassword"
