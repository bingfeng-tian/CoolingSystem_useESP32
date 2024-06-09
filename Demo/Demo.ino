#include "DHT.h"

#define DHTPIN 33
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define FAN_PIN 13
#define CRITICAL_TEMP 45

float t;

void setup()
{
  Serial.begin(9600);
  Serial.println("DHTxx test!");
  dht.begin();  //初始化DHT
  
  pinMode(FAN_PIN, OUTPUT);

} // setup()

void loop()
{
  readTemp();

  if(t > CRITICAL_TEMP) {
    digitalWrite(FAN_PIN, HIGH);
    delay(5000);
    while(t > CRITICAL_TEMP-10){
      readTemp();
    }; //持續吹風直到降到安全溫度
  }else{
    digitalWrite(FAN_PIN, LOW);
  }

} // loop()


void readTemp() {
  float h = dht.readHumidity();   //取得濕度
  t = dht.readTemperature();  //取得溫度C

  // //顯示在監控視窗裡
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" *C ");
  delay(1000);
}