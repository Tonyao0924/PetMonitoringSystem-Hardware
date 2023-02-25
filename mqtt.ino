#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// WiFi網絡參數
const char* ssid = "HITRON-C270";
const char* password = "cxz123499";

// MQTT網絡參數
const char* machineID = "TestD1";
const char* mqtt_server = "140.125.207.230";
const char* mqtt_user = "guest";
const char* mqtt_password = "guest";
const char* mqtt_topic = "weightTopic";

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

void setup() {
  // 初始化串口和GPIO引腳
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

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

  // 發送MQTT消息
  char distanceStr[10];
  sprintf(distanceStr, "%ld", distance);
  client.publish(mqtt_topic, distanceStr);

  // 等待1秒
  delay(1000);
}
