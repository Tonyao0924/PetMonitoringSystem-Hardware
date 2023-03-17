#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// WiFi網絡參數
const char* ssid = "wander_one";
const char* password = "cxz123499";

// MQTT網絡參數
const char* machineID = "TestD1";
const char* mqtt_server = "140.125.207.230";
const char* mqtt_user = "guest";
const char* mqtt_password = "guest";
const char* mqtt_topic = "weightTopic";
// 設定 LCD 顯示器的 I2C 位址和列數、行數
LiquidCrystal_I2C lcd(0x27, 16, 2);//SDA=D1腳位 SCL=D2腳位

//蜂鳴器腳位
const int buzzerPin = D6;  // 設定蜂鳴器的腳位為 D6
// 超聲波感測器連接引腳
const int trigPin = D8;
const int echoPin = D7;

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
void showDistanceOnLCD(char* distance){
    // 清空 LCD 顯示器
  lcd.clear();
  // 設定游標位置為第一列第一個位置
  lcd.setCursor(0, 0);
  // 在 LCD 顯示器上顯示文字
  lcd.print("Distance:");
  lcd.print(distance);
  // 設定游標位置為第二列第一個位置
  lcd.setCursor(0, 1);
  // 在 LCD 顯示器上顯示文字
  lcd.print("I am an Arduino.");
   // 等待 2 秒
  delay(2000);
}
void showTemperatureAndHumidityOnLCD(char* distance){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temperature:");
  lcd.print(distance);
  lcd.setCursor(0, 1);
  lcd.print("Humidity:");
  lcd.print(distance);
   // 等待 1 秒
  delay(1000);
}
void setup() {
  // 初始化串口和GPIO引腳
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  // 將蜂鳴器的腳位設為輸出模式
  pinMode(buzzerPin, OUTPUT);  
  // 初始化 LCD 顯示器
  lcd.init();
  // 打開 LCD 顯示器
  lcd.backlight();
  // 連接WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
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
  // 讀取距離
  long distance = calculateDistance();
  Serial.print("Distance: ");
  Serial.println(distance);
  char distanceStr[30];
  
  if(distance <=50){
    Serial.print("\nToo Closed\n");
    sprintf(distanceStr, "%ld", distance);
    tone(buzzerPin, 1000);  // 發送 1000 Hz 的聲音信號
    delay(500);  // 持續發送聲音信號的時間，這裡是 0.5 秒
    noTone(buzzerPin);  // 停止發送聲音信號
  }else{
    sprintf(distanceStr, "%ld", distance);
  }
  showDistanceOnLCD(distanceStr);
  // 發送MQTT消息
  client.publish(mqtt_topic, distanceStr);

  // 等待0.5秒
  delay(500);
}
