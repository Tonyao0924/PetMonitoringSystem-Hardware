import paho.mqtt.client as mqtt
import os

# 當地端程式連線伺服器得到回應時，要做的動作
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

    # 將訂閱主題寫在on_connet中
    # 如果我們失去連線或重新連線時 
    # 地端程式將會重新訂閱
    client.subscribe("topic1")

# 當接收到從伺服器發送的訊息時要進行的動作
def on_message(client, userdata, msg):
    # 轉換編碼utf-8才看得懂中文
    print(client,"User",userdata)
    print(msg.topic+" "+ msg.payload.decode('utf-8'))

# 連線設定
# 初始化地端程式
client = mqtt.Client("Test-Sub")

# 設定連線的動作
client.on_connect = on_connect

# 設定接收訊息的動作
client.on_message = on_message
USERNAME = os.getenv("MQTT_USERNAME")
PASSWORD = os.getenv("MQTT_PASSWORD")
MQTT_SERVER_IP= os.getenv("MQTT_SERVER_IP")
MQTT_SERVER_PORT= os.getenv("MQTT_SERVER_PORT")
print("SERVER_IP:",MQTT_SERVER_IP)
# 設定登入帳號密碼
client.username_pw_set(USERNAME,PASSWORD)

# 設定連線資訊(IP, Port, 連線時間)
client.connect(MQTT_SERVER_IP,int(MQTT_SERVER_PORT), 60)

# 開始連線，執行設定的動作和處理重新連線問題
# 也可以手動使用其他loop函式來進行連接
client.loop_forever()