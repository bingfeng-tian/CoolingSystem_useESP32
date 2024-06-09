#include "DHT.h"               //涵入程式庫

#define DHT11Pin 2             //感測器溫度濕度值輸入 Aeduino pin 2
#define DHTType DHT11

#include <Wire.h>               //涵入程式庫
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

DHT HT(DHT11Pin,DHTType);
float humi;                    //定義濕度 攝氏溫度 華氏溫度 的參數
float tempC;
float tempF;

//OLED define
#define SCREEN_WIDTH 128          // OLED 寬度像素
#define SCREEN_HEIGHT 64          // OLED 高度像素
                                  // Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
int fan_out;

void setup() {
  Serial.begin(9600);
  
  HT.begin();
  
  pinMode(fan_out,OUTPUT);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {      // 偵測是否安裝好OLED了
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  display.clearDisplay();           //清除畫面
}

float get_temp(){
  humi = HT.readHumidity();          //讀取濕度和溫度感測值
  tempC = HT.readTemperature();
  tempF = HT.readTemperature(true);
  return humi,tempC,tempF;
}

void display_temp(){
  Serial.print("Humidity:");         //在 Arduino 序列埠螢幕印出感測值
  Serial.print(humi,0);
  Serial.print("%");
  Serial.print(" Temperature:");
  Serial.print(tempC,1);
  Serial.print("C ~ ");
  Serial.print(tempF,1);
  Serial.println("F");
}

void display_oled(){
  //在 OLED 印出字串
  display.setTextSize(1);         // 設定文字大小
  display.setTextColor(1);        // 1:OLED預設的顏色(這個會依該OLED的顏色來決定)
  display.setCursor(0, 0);        // 設定起始座標
  display.print("Humidity");      // 要顯示的字串
  display.setCursor(60, 0);       // 設定以下要顯示的字串起始座標
  display.print("Temperature");
}
void display_humi(){
  // 印出濕度值，不帶小數點
  display.setTextSize(3);
  display.setTextColor(1);
  display.setCursor(5, 28);       // 設定要顯示的數值及字串起始座標
  display.print(humi,0);
  display.print("%");
}
void display_tempC(){
  // 印出攝氏溫度值以及小圈圈，不帶小數點
  display.setTextSize(2);
  display.setTextColor(1);
  display.setCursor(82, 16);               // 設定以下要顯示的數值起始座標
  display.print(tempC,0);
  display.drawCircle(108, 18, 2, WHITE);   // 印出溫度小圈圈
  display.setCursor(113, 16);             // 設定要顯示的字串起始座標
  display.print("C");
}
void display_tempF(){
   // 印出華氏溫度值以及小圈圈，不帶小數點
  display.setTextSize(2);
  display.setTextColor(1);
  display.setCursor(82, 44);              // 設定以下要顯示的數值起始座標
  display.print(tempF,0);
  display.drawCircle(108, 46, 2, WHITE);  // 印出溫度小圈圈 
  display.setCursor(113, 44);            // 設定要顯示的字串起始座標
  display.print("F");
}

void loop() {
 delay(1000);
 get_temp();              //取得溫度
 display_temp();         //顯示溫度

 display.clearDisplay();           //清除畫面


 display_oled();
 display_humi();
 display_tempC();
 display_tempF();
 
   display.display();                     // 要有這行才會把文字顯示出來
}