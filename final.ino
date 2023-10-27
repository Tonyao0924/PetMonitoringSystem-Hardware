
#include <Arduino.h>
#include "HX711.h"
#include <Servo.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>

Servo myServo;
HX711 scale;

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 12;
const int LOADCELL_SCK_PIN = 13;
int ledpin = 5; // D1(gpio5)
int button = 4; // D2(gpio4)
int buttonState = 0;
double firstw = 0.0;
double endw = 0.0;

const char *machineID = "TestD1";
const char *mqtt_server = "140.125.207.230";
const int mqtt_port = 1883;
const char *mqtt_user = "guest";
const char *mqtt_password = "guest";
// MQTT客戶端
WiFiClient espClient;
PubSubClient client(espClient);

const char *ssid = "DESKTOP-GOQTLT4";
const char *password = "1223334444";

void oncetime()
{
  scale.power_up();
  firstw = scale.get_units(10);

  scale.power_down();
  delay(1000);

  for (int i = 90; i > 0; i -= 3)
  {
    myServo.write(i);
  }
  //  endw = scale.get_units(10);
  //  while(endw - firstw > 100){
  //    endw = scale.get_units(10);
  //    delay(100);
  //  }
  myServo.write(0);
  for (int i = 0; i < 90; i++)
  {
    myServo.write(i);
    delay(1);
  }

  scale.power_up();
  delay(1000);
  endw = scale.get_units(10);
  scale.power_down();
  String topic = "weight/";
  String payload = String(firstw - endw);
  client.publish(topic.c_str(), payload.c_str());
}

void setup_wifi()
{
  delay(10);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }
}

void setup()
{
  setup_wifi();
  Serial.begin(115200);
  pinMode(ledpin, OUTPUT);
  pinMode(button, INPUT_PULLUP);

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  scale.set_scale(-427.2);
  scale.tare();

  myServo.attach(14); // 連接伺服馬達
  myServo.write(180);
  // 連接到MQTT伺服器
  client.setServer(mqtt_server, 1883);
  if (client.connect("NodeMCU 1.0", mqtt_user, mqtt_password))
  {
  }
  else
  {
  }
}

bool open = true;
bool alreadyopen = false;
void loop()
{
  //  buttonState = digitalRead(button); // put your main code here, to run repeatedly:
  //  if (buttonState == 0)
  //  {
  //    oncetime();
  //    delay(200);
  //  }
  firstw = scale.get_units(10);
  Serial.println(firstw);
  Serial.print(open);
  Serial.print(alreadyopen);
  if (firstw < 50 && alreadyopen == false)
  {
    open = true;
    alreadyopen = true;
    myServo.write(0);
    for (int i = 0; i < 90; i++)
    {
      myServo.write(i);
      delay(1);
    }
  }
  if (firstw > 200 && open == true)
  {
    alreadyopen = false;
    open = false;
    for (int i = 90; i > 0; i -= 3)
    {
      myServo.write(i);
      delay(1);
    }
    String topic = "weight/";
    String payload = "150";
    client.publish(topic.c_str(), payload.c_str());
  }
}
