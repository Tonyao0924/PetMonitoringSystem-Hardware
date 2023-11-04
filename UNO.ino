#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h> //LiquidCrystal_I2C
#include <Adafruit_Sensor.h>
#include <Adafruit_AHTX0.h>

Adafruit_AHTX0 aht;
LiquidCrystal_I2C lcd(0x27, 16, 2);//SDA=D1腳位 SCL=D2腳位

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
  Wire.begin();
  aht.begin();
  lcd.begin(16, 2);
  lcd.backlight();
}

void loop() {
  showMachineInfo();
  showAHT10TemperatureAndHumidityOnLCD();
  delay(2000);
}
