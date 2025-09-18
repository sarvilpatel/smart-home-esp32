#include "../include/config.h"
#include <WiFi.h>
#include <WebServer.h>
#include "DHT.h"
#include <IRremote.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ==== Pins ====
#define RELAY1 23   // Lamp
#define RELAY2 22   // Fan
#define RELAY3 21   // Device
#define PIR_PIN 19
#define DHTPIN 18
#define DHTTYPE DHT22
#define IR_PIN 17   // IR receiver

// OLED pins (custom SDA/SCL)
#define OLED_SDA 27
#define OLED_SCL 26
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ==== WiFi ====
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASS;

// ==== Globals ====
WebServer server(80);
DHT dht(DHTPIN, DHTTYPE);

// States
bool lampState = false;
bool fanState = false;
bool devState = false;
bool fanAuto = true;    // Fan starts in Auto mode
bool manualLamp = false; // Track manual lamp override
unsigned long lastMotion = 0;
float lastTempF = 0.0;

// ==== OLED update ====
void updateOLED() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.println("ESP32 Smart Home");

  display.setCursor(0, 12);
  display.print("Temp: ");
  display.print(lastTempF);
  display.println(" F");

  display.setCursor(0, 24);
  display.print("Lamp: ");
  display.println(lampState ? "ON" : "OFF");

  display.setCursor(0, 36);
  display.print("Fan: ");
  display.print(fanState ? "ON" : "OFF");
  display.print(" (");
  display.print(fanAuto ? "AUTO" : "MANUAL");
  display.println(")");

  display.setCursor(0, 48);
  display.print("Dev: ");
  display.println(devState ? "ON" : "OFF");

  display.setCursor(0, 56);
  if (WiFi.status() == WL_CONNECTED) {
    display.print(WiFi.localIP());
  } else {
    display.print("WiFi: Disconnected");
  }

  display.display();
}

// ==== Helpers ====
// Update fan immediately when in Auto mode
void updateFanAuto() {
  float tempF = dht.readTemperature(true);
  if (!isnan(tempF)) {
    lastTempF = tempF; // Save for OLED
    if (tempF > 65.0) {   // 🔥 Change back to 85.0 for final project
      fanState = true;
      digitalWrite(RELAY2, HIGH);
    } else {
      fanState = false;
      digitalWrite(RELAY2, LOW);
    }
    Serial.print("Temp(F): "); Serial.print(tempF);
    Serial.print("  → Fan: "); Serial.println(fanState ? "ON" : "OFF");
  }
  updateOLED();
}

// ==== Web Handlers ====
void handleRoot() { server.send(200, "text/html", htmlPage()); }
void lampOn()   { manualLamp = true; lampState = true; digitalWrite(RELAY1, HIGH); updateOLED(); server.sendHeader("Location","/"); server.send(303); }
void lampOff()  { manualLamp = false; lampState = false; digitalWrite(RELAY1, LOW); updateOLED(); server.sendHeader("Location","/"); server.send(303); }
void fanOn()    { fanState = true; digitalWrite(RELAY2, HIGH); updateOLED(); server.sendHeader("Location","/"); server.send(303); }
void fanOff()   { fanState = false; digitalWrite(RELAY2, LOW); updateOLED(); server.sendHeader("Location","/"); server.send(303); }
void devOn()    { devState = true; digitalWrite(RELAY3, HIGH); updateOLED(); server.sendHeader("Location","/"); server.send(303); }
void devOff()   { devState = false; digitalWrite(RELAY3, LOW); updateOLED(); server.sendHeader("Location","/"); server.send(303); }
void fanMode()  { 
  fanAuto = !fanAuto; 
  if (fanAuto) updateFanAuto();   // ✅ immediately sync fan to temp
  else updateOLED();
  server.sendHeader("Location","/"); 
  server.send(303); 
}

// ==== Web Page ====
String htmlPage() {
  String html = "<!DOCTYPE html><html><head><title>ESP32 Smart Home</title>";
  html += "<meta http-equiv='refresh' content='2'>"; // refresh every 2s
  html += "<style>body{font-family:Arial;text-align:center;margin-top:40px;}button{width:120px;height:40px;margin:8px;font-size:16px;}</style></head><body>";
  html += "<h1>ESP32 Smart Home</h1>";

  // Buttons
  html += "<button onclick=\"location.href='/lampOn'\">Lamp ON</button>";
  html += "<button onclick=\"location.href='/lampOff'\">Lamp OFF</button><br>";
  html += "<button onclick=\"location.href='/fanOn'\">Fan ON</button>";
  html += "<button onclick=\"location.href='/fanOff'\">Fan OFF</button><br>";
  html += "<button onclick=\"location.href='/devOn'\">Device ON</button>";
  html += "<button onclick=\"location.href='/devOff'\">Device OFF</button><br>";
  html += "<button onclick=\"location.href='/fanMode'\">Toggle Fan Mode</button><br><hr>";

  // States
  html += "<p>Lamp State: " + String(lampState ? "ON" : "OFF") + "</p>";
  html += "<p>Fan State: " + String(fanState ? "ON" : "OFF") + "</p>";
  html += "<p>Device State: " + String(devState ? "ON" : "OFF") + "</p>";
  html += "<p>Fan Mode: " + String(fanAuto ? "AUTO" : "MANUAL") + "</p>";

  html += "</body></html>";
  return html;
}

// ==== RTOS Tasks ====

// Web server task
void taskWeb(void *pvParameters) {
  for (;;) {
    server.handleClient();

    // ✅ WiFi reconnect safeguard
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("⚠️ WiFi lost... reconnecting");
      WiFi.disconnect();
      WiFi.begin(ssid, password);
      unsigned long startAttempt = millis();
      while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 10000) {
        vTaskDelay(500 / portTICK_PERIOD_MS);
        Serial.print(".");
      }
      if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\n✅ WiFi Reconnected! IP:");
        Serial.println(WiFi.localIP());
      }
      updateOLED();
    }

    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

// PIR task with manual override
void taskPIR(void *pvParameters) {
  for (;;) {
    if (digitalRead(PIR_PIN) == HIGH && !manualLamp) {
      lampState = true;
      digitalWrite(RELAY1, HIGH);
      lastMotion = millis();
      updateOLED();
    }
    // Auto OFF after 3s, but only if not manually ON
    if (!manualLamp && lampState && (millis() - lastMotion > 3000)) {
      lampState = false;
      digitalWrite(RELAY1, LOW);
      updateOLED();
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

// DHT task (auto fan control)
void taskDHT(void *pvParameters) {
  for (;;) {
    if (fanAuto) {
      updateFanAuto();  // keep fan synced
    } else {
      digitalWrite(RELAY2, fanState ? HIGH : LOW);
      updateOLED();
    }
    vTaskDelay(5000 / portTICK_PERIOD_MS);
  }
}

// IR Remote task
void taskIR(void *pvParameters) {
  for (;;) {
    if (IrReceiver.decode()) {
      if (IrReceiver.decodedIRData.flags != IRDATA_FLAGS_IS_REPEAT) {
        uint32_t cmd = IrReceiver.decodedIRData.command;

        if (cmd == 0x45) { // Button 1
          lampState = !lampState;
          manualLamp = lampState;  // manual overrides
          digitalWrite(RELAY1, lampState ? HIGH : LOW);
          updateOLED();
        }
        else if (cmd == 0x46) { // Button 2
          fanState = !fanState;
          digitalWrite(RELAY2, fanState ? HIGH : LOW);
          updateOLED();
        }
        else if (cmd == 0x47) { // Button 3
          devState = !devState;
          digitalWrite(RELAY3, devState ? HIGH : LOW);
          updateOLED();
        }
        else if (cmd == 0x44) { // Button 4
          fanAuto = !fanAuto;
          if (fanAuto) updateFanAuto();  // ✅ immediately sync on toggle
          else updateOLED();
          Serial.println(fanAuto ? "Fan AUTO mode" : "Fan MANUAL mode");
        }
      }
      IrReceiver.resume();
    }
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  pinMode(RELAY3, OUTPUT);
  pinMode(PIR_PIN, INPUT);

  digitalWrite(RELAY1, LOW);
  digitalWrite(RELAY2, LOW);
  digitalWrite(RELAY3, LOW);

  dht.begin();
  IrReceiver.begin(IR_PIN, ENABLE_LED_FEEDBACK);

  // OLED init with new pins
  Wire.begin(OLED_SDA, OLED_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("OLED Ready");
  display.display();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ WiFi Connected! IP:");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/lampOn", lampOn);
  server.on("/lampOff", lampOff);
  server.on("/fanOn", fanOn);
  server.on("/fanOff", fanOff);
  server.on("/devOn", devOn);
  server.on("/devOff", devOff);
  server.on("/fanMode", fanMode);
  server.begin();

  // Start RTOS tasks
  xTaskCreatePinnedToCore(taskWeb, "taskWeb", 4096, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(taskPIR, "taskPIR", 2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(taskDHT, "taskDHT", 4096, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(taskIR,  "taskIR",  2048, NULL, 1, NULL, 1);

  // Initial OLED refresh
  updateOLED();
}

void loop() {
  // RTOS handles everything
}
