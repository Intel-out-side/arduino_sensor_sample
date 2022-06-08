#include <M5Stack.h>
#include <Arduino.h>
#include <WiFi.h>
// #include <WiFiMulti.h>
#include <HTTPClient.h>
#include "Adafruit_Sensor.h"
#include <Adafruit_BMP280.h>
#include "UNIT_ENV.h"

SHT3X sht30;
QMP6988 qmp6988;
// WiFiMulti wifiMulti;
HTTPClient http;

int tmp = 0;
int hum = 0;
int prs = 0;

void setup() {
  // put your setup code here, to run once:
  M5.begin();
  M5.Power.begin();
  M5.Lcd.setTextSize(2);

  // connect to I2C slave device
  Wire.begin();
  qmp6988.init();

  // connect to Wifi
  // wifiMulti.addAP("Buffalo-G-D940", "rx7hnuyrtcuer");
  WiFi.begin("Buffalo-G-D940", "rx7hnuyrtcuer");
  while (WiFi.status() != WL_CONNECTED) {
    M5.Lcd.print(".");
    delay(500);
  }
  M5.Lcd.println("");

  // Initial message;
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.print("Connected to Wifi...\n");
  M5.Lcd.print("ENV III Unit test\n");

  delay(1000);
}

void loop() {
  prs = (int)(qmp6988.calcPressure()/100.0);

  if (sht30.get()==0) {
    tmp = (int)(sht30.cTemp);
    hum = (int)(sht30.humidity);
  }
  else {
    tmp = 0;
    hum = 0;
  }
  M5.Lcd.print("temp, hum, pres\n");
  M5.Lcd.printf("%d, %d, %d\n", tmp, hum, prs);

  String data_to_send = "{\"temperature\":" + String(tmp) + ", \"humidity\":" + String(hum) + ", \"pressure\":" + String(prs) + "}";

  M5.Lcd.println(data_to_send);
  
  if((WiFi.status() == WL_CONNECTED)) {
    // M5.Lcd.print("[HTTP] begin...");
    http.begin("https://www.teenytinybot.dev/api/status/");
    http.addHeader("Content-Type", "application/json");
    http.POST(data_to_send);
    http.end();
    M5.Lcd.print("posted!");
  } else {
    M5.Lcd.print("connection failed");
  }
  delay(60000);
  M5.Lcd.clear();
  M5.Lcd.setCursor(0,0);
  
  // put your main code here, to run repeatedly:

}
