import paho.mqtt.client as mqtt
import json  
import datetime 
import time
import os 

# 設置日期時間的格式
ISOTIMEFORMAT = '%m/%d %H:%M:%S'
clientID = "PythonTest01"

# 連線設定
# 初始化地端程式
client = mqtt.Client()

# 設定登入帳號密碼
client.username_pw_set(
    username=os.getenv("MQTT_USERNAME","guest"),
    password=os.getenv("MQTT_PASSWORD","guest")
)

# 設定連線資訊(IP, Port, 連線時間)
client.connect(
    os.getenv("MQTT_SERVER_IP","127.0.0.1"), 
    int(os.getenv("MQTT_SERVER_PORT",1883)),
    60
)

while True:
    t = datetime.datetime.now().strftime(ISOTIMEFORMAT)
    # WIFI Payload
    payload = {
        'ssid' : "12345678" ,
        "password":"12345678",
        'Time' : t
    }
    print (json.dumps(payload))
    client.publish(f"TestD1/machine", json.dumps(payload))
    time.sleep(5)