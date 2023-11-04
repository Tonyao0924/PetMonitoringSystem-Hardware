//#include <DHT.h>//Adafruit DHT sensor library
//#define DHTPIN 4     // 感測器接口鍵腳為D4
//#define DHTTYPE DHT11   // 感測器型號為DHT11
//DHT dht(DHTPIN, DHTTYPE);

//  dht.begin();//初始化 DHT11 放在 setup()
//void showDHT11TemperatureAndHumidityOnLCD(){
//  const char* mqtt_topic = "temperature/";
//  char mqttFullTopic[40]; // 定義一個字符數組用來存儲結果
//  strcpy(mqttFullTopic, mqtt_topic);
//  strcat(mqttFullTopic, machineID); 
//  float temperature = dht.readTemperature();
//  float humidity = dht.readHumidity();
//    // 檢查是否成功讀取數據
//  if (isnan(temperature) || isnan(humidity)) {
//    lcd.clear();
//    lcd.setCursor(0, 0);
//    lcd.print("Temperature:");
//    lcd.print("None");
//    lcd.setCursor(0, 1);
//    lcd.print("Humidity:");
//    lcd.print("None");
//  }else{
//    lcd.clear();
//    lcd.setCursor(0, 0);
//    lcd.print("Temperature:");
//    lcd.print(temperature);
//    lcd.setCursor(0, 1);
//    lcd.print("Humidity:");
//    lcd.print(humidity);
//  }
//  //傳送MQTT訊號
//  time_t now = time(nullptr);// Get current time
//  char date[11];
//  sprintf(date, "%04d-%02d-%02d", year(now), month(now), day(now));
//  // 創建一個JSON物件並填充數據
//  DynamicJsonDocument doc(256);
//  doc["Temperature"] = temperature;
//  doc["Humidity"] = humidity;
//  doc["Time"] = date;
//  // 將JSON物件轉換成字串
//  String json_str;
//  serializeJson(doc, json_str);
//  // 發送MQTT消息
//  client.publish(mqttFullTopic, json_str.c_str());
//  
//  delay(2000);
//}

//  showDHT11TemperatureAndHumidityOnLCD();//寫在 loop()
