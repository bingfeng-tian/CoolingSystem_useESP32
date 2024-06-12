#include <WiFi.h>
#include <WebServer.h>

#define FAN_PIN 13

/* Put your SSID & Password */
const char* ssid = "handsome_boy";  // Enter SSID here
const char* password = "12345678";  // Enter Password here

WebServer server(80);

float humi;
float temp;

bool fanstatus = LOW;

// Forward declaration of functions
void handle_OnConnect();
void handle_fanon();
void handle_fanoff();
void handle_NotFound();
String SendHTML(float humi, float temp, bool fanstat); // Corrected the parameter types

void setupWiFi() {
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  
  pinMode(FAN_PIN, OUTPUT);
  
  Serial.print("AP IP address: ");
  Serial.println(IP);
  delay(100);
  
  server.on("/", handle_OnConnect);
  server.on("/fanon", handle_fanon);
  server.on("/fanoff", handle_fanoff);
  server.onNotFound(handle_NotFound);
  
  server.begin();
  Serial.println("HTTP server started");
}

void web_loop() {
  server.handleClient();
  if (fanstatus) {
    digitalWrite(FAN_PIN, HIGH);
  } else {
    digitalWrite(FAN_PIN, LOW);
  }
}

void handle_OnConnect() {
  fanstatus = LOW;
  Serial.println("GPIO4 Status: OFF | GPIO5 Status: OFF | GPIO6 Status: OFF");
  server.send(200, "text/html", SendHTML(humi, temp, fanstatus)); 
}

void handle_fanon() {
  fanstatus = HIGH;
  Serial.println("Fan On");
  server.send(200, "text/html", SendHTML(humi, temp, fanstatus)); 
}

void handle_fanoff() {
  fanstatus = LOW;
  Serial.println("Fan Off");
  server.send(200, "text/html", SendHTML(humi, temp, fanstatus)); 
}

void handle_NotFound() {
  server.send(404, "text/plain", "Not found");
}

String SendHTML(float humi, float temp, bool fanstat) { // Corrected the parameter types
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>LED Control</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr +=".button {display: block;width: 80px;background-color: #3498db;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr +=".button-on {background-color: #3498db;}\n";
  ptr +=".button-on:active {background-color: #2980b9;}\n";
  ptr +=".button-off {background-color: #34495e;}\n";
  ptr +=".button-off:active {background-color: #2c3e50;}\n";
  ptr +="p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<h1>ESP32 Web Server</h1>\n";
  ptr +="<h3>Using Access Point(AP) Mode</h3>\n";
  
  if (fanstat) {
    ptr +="<p>Fan Status: ON</p><a class=\"button button-off\" href=\"/fanoff\">Turn OFF</a>\n";
  } else {
    ptr +="<p>Fan Status: OFF</p><a class=\"button button-on\" href=\"/fanon\">Turn ON</a>\n";
  }

  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}
