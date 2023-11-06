#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <TimeLib.h> //Time
#include <ArduinoJson.h> //ArduinoJson

const char* machineID = "TestD1";
const char *ssid = "HITRON-C270";
const char *password = "cxz123499";
const char *mqtt_server = "140.125.207.230";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

void SendWeight(float data){
  const char* mqtt_topic = "weight/";
  char mqttFullTopic[40];
  strcpy(mqttFullTopic, mqtt_topic);
  strcat(mqttFullTopic, machineID); 
  time_t now = time(nullptr);// Get current time

  char date[11];
  sprintf(date, "%04d-%02d-%02d", year(now), month(now), day(now));
  
  DynamicJsonDocument doc(256);// 創建JSON並填寫數據
  doc["Weight"] = data;
  doc["Time"] = date;
  
  String json_str;
  serializeJson(doc, json_str); // 將JSON物件轉換成字串
  client.publish(mqttFullTopic, json_str.c_str());// 發送MQTT訊號 Topic: weight/TestD1
}

void SendWater(float data){
  const char* mqtt_topic = "water/";
  char mqttFullTopic[40];
  strcpy(mqttFullTopic, mqtt_topic);
  strcat(mqttFullTopic, machineID); 
  time_t now = time(nullptr);// Get current time

  char date[11];
  sprintf(date, "%04d-%02d-%02d", year(now), month(now), day(now));
  
  DynamicJsonDocument doc(256);// 創建JSON並填寫數據
  doc["Water"] = data;
  doc["Time"] = date;
  
  String json_str;
  serializeJson(doc, json_str); // 將JSON物件轉換成字串
  client.publish(mqttFullTopic, json_str.c_str());// 發送MQTT訊號 Topic: water/TestD1
}

void SendTemperature(float data){
  const char* mqtt_topic = "temperature/";
  char mqttFullTopic[40];
  strcpy(mqttFullTopic, mqtt_topic);
  strcat(mqttFullTopic, machineID); 
  time_t now = time(nullptr);// Get current time

  char date[11];
  sprintf(date, "%04d-%02d-%02d", year(now), month(now), day(now));
  
  DynamicJsonDocument doc(256);// 創建JSON並填寫數據
  doc["Temperature"] = data;
  doc["Time"] = date;
  
  String json_str;
  serializeJson(doc, json_str); // 將JSON物件轉換成字串
  client.publish(mqttFullTopic, json_str.c_str());// 發送MQTT訊號 Topic: temperature/TestD1
}
void SendHumidity(float data){
  const char* mqtt_topic = "humidity/";
  char mqttFullTopic[40];
  strcpy(mqttFullTopic, mqtt_topic);
  strcat(mqttFullTopic, machineID); 
  time_t now = time(nullptr);// Get current time

  char date[11];
  sprintf(date, "%04d-%02d-%02d", year(now), month(now), day(now));
  
  DynamicJsonDocument doc(256);// 創建JSON並填寫數據
  doc["Humidity"] = data;
  doc["Time"] = date;
  
  String json_str;
  serializeJson(doc, json_str); // 將JSON物件轉換成字串
  client.publish(mqttFullTopic, json_str.c_str());// 發送MQTT訊號 Topic: humidity/TestD1
}

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  client.setServer(mqtt_server, mqtt_port);
  while (!client.connected()) {
    delay(500);
    Serial.print(".");
    client.connect("ESP8266Client");
  }
}

void loop() {
  while (Serial.available()) {
    String rawString = Serial.readString();//取得原始 Command 訊息
    String command = rawString.substring(0,rawString.indexOf(","));//取得感測器類別
    float data = rawString.substring(rawString.indexOf(",")+1).toFloat();//取得感測器資料
    Serial.println(command);
    if(command == "Temperature"){
      SendTemperature(data);
    }else if(command == "Humidity"){
      SendHumidity(data);
    }else if(command == "Water"){
      SendWater(data);
    }else if(command == "Weight"){
      SendWeight(data);
    }
  }
}
