#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h> //LiquidCrystal_I2C
#include <DHT.h>//Adafruit DHT sensor library
#include <ArduinoJson.h> //ArduinoJson
#include <TimeLib.h> //Time

// WiFi網絡參數
const char* ssid = "wander_one";
const char* password = "cxz123499";

// MQTT網絡參數
const char* machineID = "TestD1";
const char* mqtt_server = "140.125.207.230";
const char* mqtt_user = "guest";
const char* mqtt_password = "guest";
//const char* mqtt_topic = "weightTopic";

// 設定 LCD 顯示器的 I2C 位址和列數、行數
LiquidCrystal_I2C lcd(0x27, 16, 2);//SDA=D1腳位 SCL=D2腳位

//蜂鳴器腳位
const int buzzerPin = D6;  // 設定蜂鳴器的腳位為 D6
// 超聲波感測器連接引腳
const int trigPin = D8;
const int echoPin = D7;

#define DHTPIN D4     // 感測器接口鍵腳為D4
#define DHTTYPE DHT11   // 感測器型號為DHT11
DHT dht(DHTPIN, DHTTYPE);

// 建立WiFi客戶端和MQTT客戶端
WiFiClient espClient;
PubSubClient client(espClient);

// 計算距離的函數
long calculateDistance() {
  // 發送超聲波脈衝
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // 計算超聲波回波時間
  long duration = pulseIn(echoPin, HIGH);
  
  // 計算距離
  long distance = duration * 0.034 / 2;
  
  return distance;
}
void showConnectEroorOnLCD(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("AP Connect Error");
  lcd.setCursor(0, 1);
  lcd.print("Retype AP Info");
  delay(2000); 
}

void showDistanceOnLCD(){
  const char* mqtt_topic = "distance/";
  char mqttFullTopic[40]; // 定義一個字符數組用來存儲結果
  strcpy(mqttFullTopic, mqtt_topic);
  strcat(mqttFullTopic, machineID); 
  long distance = calculateDistance();
  char distanceStr[30];
  if(distance <=50){
    Serial.print("\nToo Closed\n");
    sprintf(distanceStr, "%ld", distance);
    tone(buzzerPin, 1000);  // 發送 1000 Hz 的聲音信號
    delay(1000);  // 持續發送聲音信號的時間，這裡是 0.5 秒
    noTone(buzzerPin);  // 停止發送聲音信號
  }else{
    sprintf(distanceStr, "%ld", distance);
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Distance:");
  lcd.print(distance);
  lcd.setCursor(0, 1);
  lcd.print("I am an Arduino.");
  
  //傳送MQTT訊號
  time_t now = time(nullptr);// Get current time
  char date[11];
  sprintf(date, "%04d-%02d-%02d", year(now), month(now), day(now));
  // 創建一個JSON物件並填充數據
  DynamicJsonDocument doc(256);
  doc["Distance"] = distance;
  doc["Time"] = date;
  // 將JSON物件轉換成字串
  String json_str;
  serializeJson(doc, json_str);
  // 發送MQTT消息
  client.publish(mqttFullTopic, json_str.c_str());
  delay(2000);// 等待 2 秒
}
void showTemperatureAndHumidityOnLCD(){
  const char* mqtt_topic = "temperature/";
  char mqttFullTopic[40]; // 定義一個字符數組用來存儲結果
  strcpy(mqttFullTopic, mqtt_topic);
  strcat(mqttFullTopic, machineID); 
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
    // 檢查是否成功讀取數據
  if (isnan(temperature) || isnan(humidity)) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temperature:");
    lcd.print("None");
    lcd.setCursor(0, 1);
    lcd.print("Humidity:");
    lcd.print("None");
  }else{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temperature:");
    lcd.print(temperature);
    lcd.setCursor(0, 1);
    lcd.print("Humidity:");
    lcd.print(humidity);
  }
  //傳送MQTT訊號
  time_t now = time(nullptr);// Get current time
  char date[11];
  sprintf(date, "%04d-%02d-%02d", year(now), month(now), day(now));
  // 創建一個JSON物件並填充數據
  DynamicJsonDocument doc(256);
  doc["Temperature"] = temperature;
  doc["Humidity"] = humidity;
  doc["Time"] = date;
  // 將JSON物件轉換成字串
  String json_str;
  serializeJson(doc, json_str);
  // 發送MQTT消息
  client.publish(mqttFullTopic, json_str.c_str());
  
  delay(2000);
}
void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);  
  lcd.init();// 初始化 LCD 顯示器
  lcd.backlight();// 打開 LCD 顯示器
  dht.begin();
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

void loop() {
  showDistanceOnLCD();
  showTemperatureAndHumidityOnLCD();
  delay(1000);// 等待1秒
}
