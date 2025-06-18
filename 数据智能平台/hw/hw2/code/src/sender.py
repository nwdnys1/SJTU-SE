import os
import pandas as pd
import json
import threading
import time

from paho.mqtt.client import Client as MQTTClient

SLEEP_TIME = 1  # 每次发送数据的间隔时间

# 读取csv文件
def load_data(folder_path: str) -> dict[str, list]:
    device_data = {}
    
    for filename in os.listdir(folder_path):
        if filename.endswith('.csv'):
            # 提取设备名（去掉.csv后缀）
            device_name = os.path.splitext(filename)[0]
            filepath = os.path.join(folder_path, filename)
            
            # 读取CSV文件
            df = pd.read_csv(filepath)
            
            # 转换为记录列表（每个记录是一个字典）
            records = df.to_dict('records')
            
            # 添加到结果字典
            device_data[device_name] = records
    
    return device_data
    


def init_mqtt(client: MQTTClient, broker: str, port: int):
    def on_connect(client, userdata, flags, rc):
        if rc == 0:
            print(f"Connected to {broker}:{port}")
        else:
            print(f"Failed to connect to {broker}:{port}, rc={rc}")

    client.on_connect = on_connect
    client.connect(broker, port)


# 仿真发送数据
def publish(device_name: str, message_list: list):
    client = MQTTClient(f"sender_{device_name}")
    init_mqtt(client, "10.119.15.62", 1883)  # 假设MQTT代理在本地运行
    for msg in message_list:
        topic_name = device_name.replace('#', '号') # 替换#为号
        binary_data = json.dumps(msg).encode('utf-8')
        client.publish(topic_name, binary_data, qos=1)
        client.loop()
        print(f"send msg to {device_name} with topic {topic_name}")
        # 每次发送数据的间隔时间，原数据间隔为 1 秒
        time.sleep(SLEEP_TIME)

# 读取数据并开始模拟发送
def main():
    data_folder = "output"
    device_data = load_data(data_folder)
    threads = []
    
    for device_name, message_list in device_data.items():

        thread = threading.Thread(target=publish, args=(device_name, message_list))
        thread.start()
        threads.append(thread)

    # 等待所有线程完成
    for thread in threads:
        thread.join()


if __name__ == "__main__":
    main()
