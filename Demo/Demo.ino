#include "DHT.h"
#include <LiquidCrystal_I2C.h>  // 引用I2C序列顯示器的程式庫
#include "web.h"

#define DHTPIN 33
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define CRITICAL_TEMP 30
#define FAN_PIN 13

// LCD顯示器
LiquidCrystal_I2C lcd(0x27, 16, 2);  // 設定模組位址0x27，以及16行, 2列的顯示形式

void setup()
{
  Serial.begin(115200);
  setupWiFi();
  
  lcd.init();       // 初始化lcd物件
  lcd.backlight();  // 開啟背光

  Serial.println("DHTxx test!");
  dht.begin();      //初始化DHT
  
  pinMode(FAN_PIN, OUTPUT);

} // setup()

void loop()
{
  readTemp();
  web_loop();

  if(temp > CRITICAL_TEMP) {
    digitalWrite(FAN_PIN, HIGH);
    delay(5000);
    while(temp > CRITICAL_TEMP-5){
      readTemp();
    }; //持續吹風直到降到安全溫度
  }else{
    digitalWrite(FAN_PIN, LOW);
  }

} // loop()


void readTemp() {
  humi = dht.readHumidity();   //取得濕度
  temp = dht.readTemperature();  //取得溫度C
  lcd.setCursor(0,0);
  lcd.print("humi:");
  lcd.print(humi);
  lcd.setCursor(0,1);
  lcd.print("temp:");
  lcd.print(temp);
  lcd.print(char(223));
  lcd.print("C");

  // //顯示在監控視窗裡
  // Serial.print("Humidity: ");
  // Serial.print(h);
  // Serial.print(" %\t");
  // Serial.print("Temperature: ");
  // Serial.print(t);
  // Serial.println(" *C ");
  delay(1000);
}