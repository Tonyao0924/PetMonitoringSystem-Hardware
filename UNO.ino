#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h> //LiquidCrystal_I2C
#include <Adafruit_Sensor.h>
#include <Adafruit_AHTX0.h>
// Weight Sensor
#include "HX711.h"
#include <Servo.h>

// LCD QRCODE
#include <SPI.h>
#include "LCD_Driver.h"
#include "GUI_Paint.h"
#include <qrcode.h>  // https://github.com/ricmoo/QRCode
unsigned char low_data[8] = {0};
unsigned char high_data[12] = {0};
#define NO_TOUCH       0xFE
#define THRESHOLD      100
#define ATTINY1_HIGH_ADDR   0x78
#define ATTINY2_LOW_ADDR   0x77

const char* pet_url = "https://peterp.page.link/Petinfo";
const char* machineID = "TestD1";
const int pumpPin = 3; //抽水馬達的繼電器連接到D11
//水位初始設置
int currentWaterLevel = 0; // 當前水位
int previousWaterLevel = 0; // 上一次的水位
// WeightSensor
const int LOADCELL_DOUT_PIN = 6;
const int LOADCELL_SCK_PIN = 5;
double firstw = 0.0;
double endw = 0.0;
bool open = true;

Servo myServo;
HX711 scale;

Adafruit_AHTX0 aht;
LiquidCrystal_I2C lcd(0x27, 16, 2);//SDA=D1腳位 SCL=D2腳位

void oncetime()
{
  scale.power_up();
  firstw = scale.get_units(10);
  Serial.println(firstw);
  if(firstw >= 200 && open == false){
    open = true;
    for (int i = 90; i > 0; i -= 3)
    {
      myServo.write(i);
    }
  }else if(firstw <= 50 && open == true){
    open = false;
    for (int i = 0; i < 90; i += 3)
    {
      myServo.write(i);
    }
  }
}

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
   qrcode_initText(&qrcode, qrcodeData, version, ECC_MEDIUM,"https://lurl.cc/wXqr99");
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
  int sensorvalue_min = 250;
  int sensorvalue_max = 255;
  int trig_section = 0;
  int water_Level ;

  getLow8SectionValue();
  getHigh12SectionValue();

  for (int i = 0 ; i < 8; i++) {
    if (low_data[i] > THRESHOLD) {
      trig_section++;
    }
  }
  for (int i = 0 ; i < 12; i++) {
    if (high_data[i] > THRESHOLD) {
      trig_section++;
    }
  }

  water_Level = trig_section * 5;
  currentWaterLevel = water_Level;
  //  currentWaterLevel = analogRead(waterSensorPin);// 讀取當前水位
  //  float voltage = currentWaterLevel * (3.3 / 1023.0); // 將數值轉換為電壓值
  //  float waterLevel = 100 - (voltage / 3.3) * 100; // 將電壓值轉換為水位百分比
   lcd.clear();
   lcd.setCursor(0, 0);
   lcd.print("Water:");
   lcd.print(water_Level);
  //  lcd.setCursor(0, 1);
  //  lcd.print("Voltage:");
  //  lcd.print(voltage);
   int waterToPump = previousWaterLevel - currentWaterLevel;

   delay(2000);
   String waterCommand = String("Water") + "," + String((float)water_Level);//減少的水位:
   Serial.println(waterCommand);


   if (currentWaterLevel < previousWaterLevel) {
     
     // 啟動抽水馬達以補充水位
     digitalWrite(pumpPin, LOW);
     delay(water_Level*1000/24); // 設定抽水時間
     digitalWrite(pumpPin, HIGH); // 停止抽水
   }
   // 上傳減少的水量到MQTT

   previousWaterLevel = currentWaterLevel;// 更新上一次的水位
}

void getHigh12SectionValue(void)
{
  memset(high_data, 0, sizeof(high_data));
  Wire.requestFrom(ATTINY1_HIGH_ADDR, 12);
  while (12 != Wire.available());

  for (int i = 0; i < 12; i++) {
    high_data[i] = Wire.read();
  }
  delay(10);
}

void getLow8SectionValue(void)
{
  memset(low_data, 0, sizeof(low_data));
  Wire.requestFrom(ATTINY2_LOW_ADDR, 8);
  while (8 != Wire.available());

  for (int i = 0; i < 8; i++) {
    low_data[i] = Wire.read();
  }
  delay(10);
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
   delay(2000);
   
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
 pinMode(pumpPin, OUTPUT);
 Wire.begin();
 aht.begin();
 lcd.begin(16, 2);
 lcd.backlight();
 
 open = true;
 scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

 scale.set_scale(-427.2);
 scale.tare();

 myServo.attach(4); // 連接伺服馬達
 createQRCode();
}

void loop() {
 showMachineInfo();
 showAHT10TemperatureAndHumidityOnLCD();
 monitorWater();
 oncetime();
 delay(2000);
}
