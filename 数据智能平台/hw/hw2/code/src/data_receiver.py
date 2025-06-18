from paho.mqtt.client import Client as MQTTClient
from paho.mqtt.client import MQTTMessage
import json
from pyspark.sql import SparkSession
from pyspark.sql.types import StructType, StructField, StringType, DoubleType, LongType
from delta import *
from queue import Queue
from threading import Lock
from typing import Dict, List
import threading
import time

MQTT_CLIENT_ID = f"data_receiver"
MQTT_SUB_TOPIC = "#"  # 订阅所有主题
BATCH_INTERVAL = 30  # 批处理间隔(秒)

# 全局数据结构用于缓存消息
buffer = Queue()
buffer_lock = Lock()
device_schemas: Dict[str, StructType] = {}


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
        try:
            data = json.loads(msg.payload.decode("utf-8"))

            with buffer_lock:
                buffer.put((device_name, data))

        except Exception as e:
            print(f"Error processing message: {str(e)}")
        # 初始化MQTT客户端

    client = MQTTClient(MQTT_CLIENT_ID, clean_session=False)
    client.on_message = on_message
    client.connect("localhost", 1883)
    client.subscribe(MQTT_SUB_TOPIC)

    return client


# 运行mqtt监听
def run_mqtt():
    client = init_mqtt()
    client.loop_forever()


# 初始化Spark会话
def init_spark():
    builder = (
        SparkSession.builder.appName("EnergyDataAnalysis")
        .config("spark.sql.extensions", "io.delta.sql.DeltaSparkSessionExtension")
        .config(
            "spark.sql.catalog.spark_catalog",
            "org.apache.spark.sql.delta.catalog.DeltaCatalog",
        )
    )

    # 直接设置日志级别
    spark = builder.getOrCreate()
    spark.sparkContext.setLogLevel("WARN")  # 只显示WARN及以上级别

    return spark


# 批处理写入函数
def batch_write_to_delta(spark: SparkSession):
    while True:
        time.sleep(BATCH_INTERVAL)

        if buffer.empty():
            continue

        # 收集批处理数据
        batch_data: Dict[str, List[Dict]] = {}
        batch_size = 0

        with buffer_lock:
            while not buffer.empty():
                device_name, record = buffer.get()
                if device_name not in batch_data:
                    batch_data[device_name] = []
                batch_data[device_name].append(record)
                batch_size += 1

        if not batch_data:
            continue

        try:
            # 为每个设备写入数据
            for device_name, records in batch_data.items():
                # 推断或获取schema
                if device_name not in device_schemas:
                    sample_record = records[0]
                    fields = []
                    for key, value in sample_record.items():
                        if isinstance(value, int):
                            fields.append(StructField(key, LongType(), True))
                        elif isinstance(value, float):
                            fields.append(StructField(key, DoubleType(), True))
                        else:
                            fields.append(StructField(key, StringType(), True))
                    device_schemas[device_name] = StructType(fields)

                # 创建DataFrame并写入
                df = spark.createDataFrame(records, device_schemas[device_name])

                table_path = f"/tmp/delta_tables/{device_name}"

                # 使用Delta的流式写入
                df.write.format("delta").mode("append").option("delta.columnMapping.mode", "name") \
                    .option("delta.minReaderVersion", "2") \
                    .option("delta.minWriterVersion", "5") \
                    .save(table_path)

                print(
                    f"Successfully wrote {len(records)} records to {device_name} table"
                )

        except Exception as e:
            print(f"Error in batch write: {str(e)}")
            # 可以考虑将失败的数据重新放回队列


if __name__ == "__main__":

    spark = init_spark()
    t1 = threading.Thread(target=run_mqtt)
    t1.start()
    t2 = threading.Thread(target=batch_write_to_delta, args=(spark,))
    t2.start()
