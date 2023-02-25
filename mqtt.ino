#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.
const char* machineID = "TestD1";
const char* ssid = "HITRON-C270";
const char* password = "cxz123499";
const char* mqtt_server = "140.125.207.230"; 
const char* mqtt_user = "guest";
const char* mqtt_pass= "guest";
const char* weightTopic = "weightTopic";

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  // Connecting to a WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  Serial.println("In reconnect...");
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(machineID, mqtt_user, mqtt_pass)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void loop() {
  char* msg = "測試Test";
  if (!client.connected()) {
    reconnect();
  }

  client.publish(weightTopic, msg);
  Serial.print("MQ2 gas value:");
  Serial.println(msg);

  delay(5000);
}
