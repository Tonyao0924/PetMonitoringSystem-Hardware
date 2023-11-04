#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h> //LiquidCrystal_I2C
#include "Adafruit_AHTX0.h"  // AHT10 library
#include <ArduinoJson.h> //ArduinoJson
#include <TimeLib.h> //Time
#include <Arduino.h>
#include "HX711.h"
#include <Servo.h>

// WiFi網絡參數
const char* ssid = "wander_one";
const char* password = "cxz123499";

// MQTT網絡參數
const char* machineID = "TestD1";
const char* mqtt_server = "140.125.207.230";
const char* mqtt_user = "guest";
const char* mqtt_password = "guest";

// 設定水位感測器腳位為D8
const int waterSensorPin = 8; 
//水位初始設置
int currentWaterLevel = 0; // 當前水位
int previousWaterLevel = 0; // 上一次的水位

void showWaterOnLCD(){
  int sensorValue = analogRead(waterSensorPin); // 讀取水位感測器數值
  float voltage = sensorValue * (3.3 / 1023.0); // 將數值轉換為電壓值
  float waterLevel = 100 - (voltage / 3.3) * 100; // 將電壓值轉換為水位百分比
  Serial.print("waterLevel");
  Serial.print(waterLevel);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Water:");
  lcd.print(waterLevel);
  lcd.setCursor(0, 1);
  lcd.print("Voltage:");
  lcd.print(voltage);
  delay(2000);
}

void setup() {
  Serial.begin(115200);
  Serial.println("Adafruit AHT10/AHT20 demo!");
  // 初始化水位感測器和抽水馬達
  pinMode(waterSensorPin, INPUT);
  pinMode(pumpPin, OUTPUT);

  // 連接WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
    showConnectEroorOnLCD();
  }
  Serial.println("Connected to WiFi");

  // 連接MQTT服務器
  client.setServer(mqtt_server, 1883);
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ESP8266Client", mqtt_user, mqtt_password )) {
      Serial.println("Connected to MQTT");
    } else {
      Serial.print("Failed to connect to MQTT, rc=");
      Serial.print(client.state());
      Serial.println(" retrying in 5 seconds");
      delay(5000);
    }
  }
}

void monitorWater(){
   // 讀取當前水位
  currentWaterLevel = analogRead(waterSensorPin);
  float voltage = currentWaterLevel * (3.3 / 1023.0); // 將數值轉換為電壓值
  float waterLevel = 100 - (voltage / 3.3) * 100; // 將電壓值轉換為水位百分比
  Serial.print("waterLevel");
  Serial.print(waterLevel);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Water:");
  lcd.print(waterLevel);
  lcd.setCursor(0, 1);
  lcd.print("Voltage:");
  lcd.print(voltage);
  
  if (currentWaterLevel < previousWaterLevel) {
    int waterToPump = previousWaterLevel - currentWaterLevel;
    // 上傳減少的水量到MQTT
    String topic = "water/";
    String payload = String(waterToPump);
     Serial.print("減少的水位:");
      Serial.println(waterToPump);
    client.publish(topic.c_str(), payload.c_str());

    // 啟動抽水馬達以補充水位
    digitalWrite(pumpPin, LOW);
    delay(waterLevel*1000/24); // 設定抽水時間
    digitalWrite(pumpPin, HIGH); // 停止抽水
  }

  // 更新上一次的水位
  previousWaterLevel = currentWaterLevel;
}
  
void loop() {
  showWaterOnLCD();
}
