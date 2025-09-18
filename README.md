# ESP32 Smart Home Controller

ESP32 smart home project with:
- Web UI to control Lamp, Fan, and Device
- PIR auto lamp with manual override
- DHT22 auto fan (temperature based)
- IR remote control
- OLED display for live status

## 🎥 Demo
[![Watch the demo](https://img.youtube.com/vi/7I54fgAhxs4/0.jpg)](https://youtu.be/7I54fgAhxs4)

## 🚀 Getting Started
1. Copy `include/config.example.h` → `include/config.h`  
   Add your Wi-Fi SSID and password there.  
2. Open `src/main.ino` in Arduino IDE.  
3. Install required libraries (Adafruit GFX, SSD1306, DHT, IRremote).  
4. Upload to ESP32.  

## 🔧 Hardware
- ESP32 Dev Board  
- 3-Channel Relay Module  
- PIR Sensor  
- DHT22 Sensor  
- SSD1306 OLED (128x64, I2C)  
- IR Receiver  
