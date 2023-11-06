#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h> //LiquidCrystal_I2C
#include <Adafruit_Sensor.h>
#include <Adafruit_AHTX0.h>

// LCD QRCODE
#include <SPI.h>
#include "LCD_Driver.h"
#include "GUI_Paint.h"
#include <qrcode.h>  // https://github.com/ricmoo/QRCode

const char* machineID = "TestD1";
const int waterSensorPin = 8;  // 設定水位感測器腳位為D8
const int pumpPin = 5; //抽水馬達的繼電器連接到D1
//水位初始設置
int currentWaterLevel = 0; // 當前水位
int previousWaterLevel = 0; // 上一次的水位

Adafruit_AHTX0 aht;
LiquidCrystal_I2C lcd(0x27, 16, 2);//SDA=D1腳位 SCL=D2腳位

// LCD QRCODE
void createQRCode(){
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

void monitorWater(){
   currentWaterLevel = analogRead(waterSensorPin);// 讀取當前水位
   float voltage = currentWaterLevel * (3.3 / 1023.0); // 將數值轉換為電壓值
   float waterLevel = 100 - (voltage / 3.3) * 100; // 將電壓值轉換為水位百分比
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
     String waterCommand = String("Water") + "," + String(waterToPump);//減少的水位:
     Serial.println(waterCommand);
 
     // 啟動抽水馬達以補充水位
     digitalWrite(pumpPin, LOW);
     delay(waterLevel*1000/24); // 設定抽水時間
     digitalWrite(pumpPin, HIGH); // 停止抽水
   }
   previousWaterLevel = currentWaterLevel;// 更新上一次的水位
}

void showAHT10TemperatureAndHumidityOnLCD(){
   sensors_event_t humidity, temp;
   aht.getEvent(&humidity, &temp);// 讀取溫濕度數據

   float temperatureData = temp.temperature;
   float humidityData = humidity.relative_humidity;
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

   String temperatureCommand = String("Temperature") + "," + String(temperatureData);
   Serial.println(temperatureCommand);//發送溫度指令
   delay(2000);
   String humidityCommand = String("Humidity") + "," + String(humidityData);
   Serial.println(humidityCommand);//發送濕度指令
   
}

void showMachineInfo(){
 lcd.clear();
 lcd.setCursor(0, 0);
 lcd.setCursor(0, 1);
 lcd.print("MachineID ");
 lcd.print(machineID);
 delay(2000);// 等待 2 秒
}

void setup() {
 Serial.begin(115200);
 // 初始化水位感測器和抽水馬達
 pinMode(waterSensorPin, INPUT);
 pinMode(pumpPin, OUTPUT);
 Wire.begin();
 aht.begin();
 lcd.begin(16, 2);
 lcd.backlight();
 
 createQRCode();
}

void loop() {
 showMachineInfo();
 showAHT10TemperatureAndHumidityOnLCD();
 monitorWater();
 delay(2000);
}
