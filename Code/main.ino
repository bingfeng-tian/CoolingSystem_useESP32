#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>

// --- 腳位與參數設定 ---
#define DHTPIN 33
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

#define CRITICAL_TEMP 35
#define FAN_PIN 13        // 模擬器內用藍燈替代風扇
#define LED_RED 14        // 過熱提示
#define LED_GREEN 12      // 溫度正常

// --- WiFi 與 WebServer 設定 ---
const char* ssid = "Wokwi-GUEST";       // Wokwi 專用的模擬 WiFi
const char* password = "";              // Wokwi 模擬 WiFi 沒有密碼
const uint32_t WIFI_TIMEOUT_MS = 10000; // 連線逾時 10 秒
bool isWiFiConnected = false;           // 紀錄是否連上網路

WebServer server(80);

float humi = 0.0;
float temp = 0.0;
bool isManualMode = false;   
bool manualFanOn = false;         // 手動模式，風扇運作狀態
String fanStatusString = "關閉";  // 風扇當前狀態，利於前端顯示

LiquidCrystal_I2C lcd(0x27, 16, 2);

unsigned long previousMillis = 0;
const long interval = 2000; 

void setup() {
  Serial.begin(115200);
  
  pinMode(FAN_PIN, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  
  lcd.init();       
  lcd.backlight();  
  dht.begin();

  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  lcd.setCursor(0, 0);
  lcd.print("WiFi Connecting.");
  
  // --- WiFi 連線邏輯（含逾時判斷）---
  Serial.println("正在嘗試連線至 WiFi...");
  lcd.setCursor(0, 0);
  lcd.print("WiFi connecting");
  
  WiFi.begin(ssid, password);
  unsigned long startAttemptTime = millis();

  // 同時檢查連線狀態與是否超過 10 秒
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < WIFI_TIMEOUT_MS) {
    delay(500);
    Serial.print(".");
    lcd.setCursor(14, 0);
    lcd.print(".."); // 顯示動態小點
  }

  if (WiFi.status() == WL_CONNECTED) {
    isWiFiConnected = true;
    Serial.println("\nWiFi 連線成功！");
    Serial.print("IP 位址: ");
    Serial.println(WiFi.localIP());

    lcd.clear();
    lcd.print("Online Mode");
    lcd.setCursor(0, 1);
    lcd.print(WiFi.localIP());

    // 只有連線成功才啟動 WebServer
    server.on("/", handleRoot);
    server.on("/fan_on", handleFanOn);
    server.on("/fan_off", handleFanOff);
    server.on("/auto_mode", handleAutoMode);
    server.begin();
  } else {
    isWiFiConnected = false;
    WiFi.disconnect(true); // 停止連線嘗試
    WiFi.mode(WIFI_OFF);   // 關閉 WiFi 功能以省電
    Serial.println("\nWiFi 連線逾時，進入離線模式。");
    
    lcd.clear();
    lcd.print("Offline Mode");
    lcd.setCursor(0, 1);
    lcd.print("No Network");
  }
  
  delay(3000); 
  lcd.clear();
} 

void loop() {
// 只有在網路連線時才處理網頁請求
  if (isWiFiConnected) {
    server.handleClient();
  }

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    readTemp();
    controlSystem();
    showData(humi, temp);
  }
} 

void readTemp() {
  humi = dht.readHumidity();
  temp = dht.readTemperature();  
}

void controlSystem() {
  if (isManualMode) {
    if (manualFanOn) {
      digitalWrite(FAN_PIN, HIGH);
      fanStatusString = "開啟 (手動)";
    } else {
      digitalWrite(FAN_PIN, LOW);
      fanStatusString = "關閉 (手動)";
    }
    updateLEDs(temp > CRITICAL_TEMP);
  } else {
    if (temp > CRITICAL_TEMP) {
      digitalWrite(FAN_PIN, HIGH);
      updateLEDs(true);
      fanStatusString = "開啟 (自動降溫中)";
    } else if (temp <= CRITICAL_TEMP - 5) {
      digitalWrite(FAN_PIN, LOW);
      updateLEDs(false);
      fanStatusString = "關閉 (自動待機)";
    } else {
      if (digitalRead(FAN_PIN) == HIGH) {
         updateLEDs(true);
         fanStatusString = "開啟 (自動降溫中)";
      } else {
         updateLEDs(false);
         fanStatusString = "關閉 (自動待機)";
      }
    }
  }
}

void updateLEDs(bool isTooHot) {
  if (isTooHot) {
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_GREEN, LOW);
  } else {
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_GREEN, HIGH);
  }
}

void showData(float h, float t) {
  if (isnan(h) || isnan(t)) return;

  lcd.setCursor(0, 0);
  lcd.print("Humi:");
  lcd.print(h);
  lcd.print("%  ");
  
  lcd.setCursor(0, 1);
  lcd.print("Temp:");
  lcd.print(t);
  lcd.print(char(223));
  lcd.print("C  ");

  Serial.print("Temp: "); Serial.print(t);
  Serial.println("\tFan: " + fanStatusString);
}

void handleRoot() {
  String html = "<html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'><meta http-equiv='refresh' content='5'>"; 
  html += "<style>body{font-family: Arial, sans-serif; text-align: center; margin-top: 50px; background-color: #f4f4f4;}";
  html += "h1{color: #333;} .card{background: white; padding: 20px; border-radius: 10px; display: inline-block; box-shadow: 0 4px 8px rgba(0,0,0,0.1);}";
  html += ".btn{padding: 10px 20px; font-size: 18px; margin: 10px; cursor: pointer; border: none; border-radius: 5px; color: white;}";
  html += ".btn-on{background-color: #e74c3c;} .btn-off{background-color: #95a5a6;} .btn-auto{background-color: #3498db;}</style></head><body>";
  html += "<h1>ESP32 散熱系統監控面板</h1><div class='card'><h2>環境資訊</h2>";
  html += "<p style='font-size: 24px;'>🌡️ 溫度: <b>" + String(temp) + " &deg;C</b></p>";
  html += "<p style='font-size: 24px;'>💧 濕度: <b>" + String(humi) + " %</b></p>";
  html += "<p style='font-size: 20px;'>🌀 風扇狀態: <b>" + fanStatusString + "</b></p><hr><h2>遠端控制</h2>";
  html += "<a href='/fan_on'><button class='btn btn-on'>手動開啟風扇</button></a>";
  html += "<a href='/fan_off'><button class='btn btn-off'>手動關閉風扇</button></a><br>";
  html += "<a href='/auto_mode'><button class='btn btn-auto'>恢復自動模式</button></a></div></body></html>";
  server.send(200, "text/html", html);
}

void handleFanOn() { isManualMode = true; manualFanOn = true; controlSystem(); server.sendHeader("Location", "/"); server.send(303); }
void handleFanOff() { isManualMode = true; manualFanOn = false; controlSystem(); server.sendHeader("Location", "/"); server.send(303); }
void handleAutoMode() { isManualMode = false; controlSystem(); server.sendHeader("Location", "/"); server.send(303); }