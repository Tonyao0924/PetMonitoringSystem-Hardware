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



// LCD QRCODE
#include <SPI.h>
#include "LCD_Driver.h"
#include "GUI_Paint.h"
#include <qrcode.h>  // https://github.com/ricmoo/QRCode

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
//抽水馬達的繼電器連接到D1
const int pumpPin = 5;
//水位初始設置
int currentWaterLevel = 0; // 當前水位
int previousWaterLevel = 0; // 上一次的水位

//重量初始化設定

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 12;
const int LOADCELL_SCK_PIN = 13;
double firstw = 0.0;
double endw = 0.0;
bool open = true;
bool alreadyopen = false;

// 實例化 Sensor 物件
Adafruit_AHTX0 aht;
WiFiClient espClient;
PubSubClient client(espClient);
LiquidCrystal_I2C lcd(0x27, 16, 2);//SDA=D1腳位 SCL=D2腳位
Servo myServo;
HX711 scale;


void oncetime() {
  firstw = scale.get_units(10);
  Serial.println(firstw);
  Serial.print(open);
  Serial.print(alreadyopen);
  if (firstw < 50 && alreadyopen == false) {
    open = true;
    alreadyopen = true;
    myServo.write(0);
    for (int i = 0; i < 90; i++) {
      myServo.write(i);
      delay(1);
    }
  }
  if (firstw > 200 && open == true) {
    alreadyopen = false;
    open = false;
    for (int i = 90; i > 0; i -= 3) {
      myServo.write(i);
      delay(1);
    }
  }

}

void showAHT10TemperatureAndHumidityOnLCD(){
    // 讀取溫濕度數據
    sensors_event_t humidity, temp;
    aht.getEvent(&humidity, &temp);
    const char* mqtt_topic = "temperature/";
    char mqttFullTopic[40]; // 定義一個字符數組用來存儲結果
    strcpy(mqttFullTopic, mqtt_topic);
    strcat(mqttFullTopic, machineID); 
    // 檢查是否成功讀取數據 輸出數據
    if (isnan(temp.temperature) || isnan(humidity.relative_humidity)) {
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
      lcd.print(temp.temperature);
      lcd.print("°C");
      lcd.setCursor(0, 1);
      lcd.print("Humidity:");
      lcd.print(humidity.relative_humidity);
      lcd.print("%");
    }
    //傳送MQTT訊號
    time_t now = time(nullptr);// Get current time
    char date[11];
    sprintf(date, "%04d-%02d-%02d", year(now), month(now), day(now));
    // 創建一個JSON物件並填充數據
    DynamicJsonDocument doc(256);
    doc["Temperature"] = temp.temperature;
    doc["Humidity"] = humidity.relative_humidity;
    doc["Time"] = date;
    // 將JSON物件轉換成字串
    String json_str;
    serializeJson(doc, json_str);
    // 發送MQTT消息
    client.publish(mqttFullTopic, json_str.c_str());
    delay(2000);
}


void showConnectEroorOnLCD(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("AP Connect Error");
  lcd.setCursor(0, 1);
  lcd.print("Retype AP Info");
  delay(2000); 
}
// LCD QRCODE
void CreateQRcode(){
        Config_Init();
        LCD_Init();
        LCD_SetBacklight(100); 
    int version = 2;
      Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, 90, WHITE);
      Paint_SetRotate(ROTATE_0);
      Paint_Clear(WHITE);
    QRCode qrcode;
      uint8_t qrcodeData[qrcode_getBufferSize(version)];
      qrcode_initText(&qrcode, qrcodeData, version, ECC_MEDIUM, "https://www.google.com/");
    int offset_x = 35;
    int offset_y = 60;
      for (int y = 0; y < qrcode.size; y++) {
        for (int x = 0; x < qrcode.size; x++) {
          int newX = offset_x + (x * 4);
          int newY = offset_y + (y * 4);
          if(qrcode_getModule(&qrcode, x, y)){
            Paint_DrawRectangle(newX, newY, newX+4, newY+4, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
          }else{
            Paint_DrawRectangle(newX, newY, newX+4, newY+4, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
          }
        }
      }
}



void setup() {
  Serial.begin(115200);
  Serial.println("Adafruit AHT10/AHT20 demo!");

  // 初始化水位感測器和抽水馬達
  pinMode(waterSensorPin, INPUT);
  pinMode(pumpPin, OUTPUT);
  //Initialize
  Serial.println("Before Init Object");
  aht.begin();
  lcd.begin(16, 2);
  lcd.backlight();
  Serial.println("After Init Object");

  //重量setUp
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  scale.set_scale(-427.2);
  scale.tare();

  myServo.attach(14); // 連接伺服馬達

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
  CreateQRcode();
}
void showMachineInfo(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.setCursor(0, 1);
  lcd.print("MachineID ");
  lcd.print(machineID);
  delay(2000);// 等待 2 秒
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
  showMachineInfo();
  showAHT10TemperatureAndHumidityOnLCD();
  monitorWater();
  oncetime();

  delay(1000);// 等待1秒
}
