from flask import Flask, jsonify
from pyspark.sql import SparkSession
import time
from py4j.java_gateway import java_import
from flask import request
from paho.mqtt.client import Client as MQTTClient
from paho.mqtt.client import MQTTMessage
import json
import threading

app = Flask(__name__)

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


# 初始化 SparkSession
def init_spark():
    spark = (
        SparkSession.builder.appName("FlaskDeltaLakeApp")
        .config("spark.master", "local")
        .config("spark.submit.deployMode", "client")
        .config("spark.sql.extensions", "io.delta.sql.DeltaSparkSessionExtension")
        .config(
            "spark.sql.catalog.spark_catalog",
            "org.apache.spark.sql.delta.catalog.DeltaCatalog",
        )
        .config("spark.jars.packages", "io.delta:delta-core_2.13:2.4.0")
        .config("spark.jars.repositories", "https://repo1.maven.org/maven2")
        .getOrCreate()
    )
    return spark


# 获取 HDFS 目录下的所有 Delta 表文件信息
def list_delta_tables_info(spark, hdfs_path):
    hadoop_conf = spark.sparkContext._jsc.hadoopConfiguration()
    path = spark._jvm.org.apache.hadoop.fs.Path(hdfs_path)
    fs = path.getFileSystem(hadoop_conf)

    statuses = fs.listStatus(path)
    delta_tables_info = []
    for status in statuses:
        if status.isDirectory():
            folder_path = status.getPath().toString()
            # 检查文件夹下是否包含 _delta_log
            if fs.exists(
                spark._jvm.org.apache.hadoop.fs.Path(folder_path + "/_delta_log")
            ):
                table_name = status.getPath().getName()
                delta_tables_info.append(
                    {"table_name": table_name, "table_path": folder_path}
                )

    return delta_tables_info


@app.route("/delta_tables", methods=["GET"])
def get_delta_tables():
    spark = init_spark()
    base_path = "/tmp/delta_tables"  # HDFS 路径

    # 获取 HDFS 目录下的所有 Delta 表文件信息
    delta_tables_info = list_delta_tables_info(spark, base_path)
    print(f"HDFS 目录下的 Delta 表信息: {delta_tables_info}")

    # 返回 JSON 格式的表信息
    return jsonify(delta_tables_info)


# 读取指定设备的所有 Delta 表数据并合并
def read_device_data(spark, device_table_paths):
    dfs = []
    for table_path in device_table_paths:
        df = spark.read.format("delta").load(table_path)
        dfs.append(df)
        df.show()  # 显示每个表的数据

    if dfs:
        combined_df = dfs[0]
        for df in dfs[1:]:
            combined_df = combined_df.union(df)
        return combined_df
    else:
        return None


@app.route("/device_data", methods=["GET"])
def get_device_data():
    spark = init_spark()
    base_path = "/tmp/delta_tables"  # HDFS 基础路径
    device_name = request.args.get("device_name")

    if not device_name:
        return jsonify({"error": "device_name parameter is required"}), 400

    # 获取指定设备的所有 Delta 表路径
    device_table_paths = [base_path + "/" + device_name]
    if not device_table_paths:
        return (
            jsonify({"error": f"No Delta tables found for device: {device_name}"}),
            404,
        )

    print(f"Found Delta tables for device {device_name}: {device_table_paths}")

    # 读取并合并设备数据
    combined_df = read_device_data(spark, device_table_paths)

    if combined_df is None or combined_df.rdd.isEmpty():
        return jsonify({"error": "No data found for the specified device"}), 404

    # 动态获取列名并返回所有数据
    columns = combined_df.columns  # 获取所有列名
    result_df = combined_df.select(columns)  # 选择所有列

    # 将 DataFrame 转换为 Pandas DataFrame 以便返回 JSON
    result_pd = result_df.toPandas()
    result_json = result_pd.to_json(orient="records", date_format="iso")

    return result_json, 200, {"Content-Type": "application/json"}


if __name__ == "__main__":

    t1 = threading.Thread(target=run_mqtt)
    t1.start()

    app.run(host="0.0.0.0", port=5000)
