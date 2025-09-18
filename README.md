# 🏠 ESP32 Smart Home Controller

[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)

ESP32-based smart home project featuring:  
✅ Web UI to control Lamp, Fan, and Device  
✅ PIR auto lamp with manual override  
✅ DHT22 auto fan (temperature-based)  
✅ IR remote control  
✅ OLED display for live system status  

---

## 🎥 Demo  
[![Watch the video]([https://img.youtube.com/vi/7154fgAhxs4/0.jpg)](https://youtu.be/7154fgAhxs4](https://youtu.be/7I54fgAhxs4?si=R3BalJW8MDk16-WI))

---

## 🚀 Getting Started

1. Copy `include/config.example.h` → `include/config.h`  
   Add your Wi-Fi **SSID** and **password** there.  
2. Open `src/main.ino` in Arduino IDE.  
3. Install required libraries:  
   - `Adafruit GFX`  
   - `Adafruit SSD1306`  
   - `DHT`  
   - `IRremote`  
4. Connect your ESP32 and upload.

---

## 🔧 Hardware

| Component              | Purpose                  |
|------------------------|--------------------------|
| ESP32 Dev Board        | Main controller          |
| 3-Channel Relay Module | Controls Lamp, Fan, Device |
| PIR Sensor             | Motion detection (lamp) |
| DHT22 Sensor           | Temperature/humidity fan control |
| SSD1306 OLED (128×64)  | Live system display      |
| IR Receiver            | Remote input             |

---

## 📜 License  
This project is licensed under the [MIT License](LICENSE).  

👤 Author: **Sarvil Patel**  
