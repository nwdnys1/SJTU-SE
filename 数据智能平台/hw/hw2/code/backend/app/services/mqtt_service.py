from py4j.java_gateway import java_import
from flask import request
from paho.mqtt.client import Client as MQTTClient
from paho.mqtt.client import MQTTMessage
import json
import threading

MQTT_CLIENT_ID = f"data_receiver"
MQTT_SUB_TOPIC = "#"  # 订阅所有主题

realtime_data = {}  # 用于存储实时数据
# 比如<CustomGauge title={"2号站三联供总供热"} unit={"kWh"} total={30000} />
# realtime_data["2号站三联供"]["热总能量计累计热量"]
#  <CustomGauge title={"2号站热水供水总管温度"} unit={"℃"} total={100} />
# realtime_data["2号站热水供水总管"]["温度"]

# 建立mqtt连接
def init_mqtt() -> MQTTClient:
    def on_connect(_client, _userdata, _flags, rc):
        if rc == 0:
            print("Connected to MQTT Broker!")
        else:
            print("Failed to connect, return code %d\n", rc)

    # MQTT消息处理
    def on_message(_client, _userdata, msg: MQTTMessage):
        device_name = msg.topic
        data = json.loads(msg.payload.decode("utf-8"))
        realtime_data[device_name] = data

    client = MQTTClient(MQTT_CLIENT_ID, clean_session=False)
    client.on_message = on_message
    client.connect("localhost", 1883)
    client.subscribe(MQTT_SUB_TOPIC)

    return client

# 运行mqtt监听
def run_mqtt():
    client = init_mqtt()
    client.loop_forever()
