from flask import Blueprint, request, jsonify
from app.services.spark_service import SparkService
from app.services.delta_table_service import DeltaTableService
from pyspark.sql import DataFrame
from app.utils.response import ApiResponse
import pandas as pd

# 创建 device 蓝图
bp = Blueprint('device', __name__)

# 获取所有device的名称
@bp.route('/all_device_names', methods=['GET'])
def get_delta_tables():
    base_path = "/tmp/delta_tables"  # HDFS 路径
    delta_tables_info = DeltaTableService.list_delta_tables_info(base_path)
    
    # 根据设备名称进行排序
    # delta_tables_info.sort(key=lambda x: x['device_name'])

    return ApiResponse(data=delta_tables_info, message="获取所有设备名称成功", code=200).to_json()

# 获取指定设备的所有 Delta 表数据并合并
@bp.route('/device_data', methods=['GET'])
def get_device_data():
    device_name = request.args.get('device_name')
    month = request.args.get('month')

    if not device_name:
        return jsonify({"error": "device_name parameter is required"}), 400
    
    if not month:
        return jsonify({"error": "month parameter is required"}), 400

    base_path = "/tmp/delta_tables"  # HDFS 基础路径
    table_path = f"{base_path}/{device_name}"  # 直接构造单个路径

    # 读取 Delta 表
    spark = SparkService.get_spark_session()
    combined_df = spark.read.format("delta").load(table_path)

    if combined_df is None or combined_df.rdd.isEmpty():
        return jsonify({"error": "No data found for the specified device"}), 404

    # 将 Spark DataFrame 转换为 Pandas DataFrame
    result_pd = combined_df.toPandas()

    # 将 timestamp 列转换为 datetime 类型
    result_pd['timestamp'] = pd.to_datetime(result_pd['timestamp'])

    # 根据 timestamp 列进行排序
    result_pd = result_pd.sort_values(by='timestamp')

    # 遍历 DataFrame 的每一行，将其转换为字典格式，同时根据月份进行过滤
    result_json = []
    row_count = 0
    for index, row in result_pd.iterrows():
        row_count += 1
        #将这一行中的timestamp读取出来
        timestamp = row['timestamp'].strftime('%Y-%m-%dT%H:%M:%SZ')
        if timestamp[5:7] != str(month).zfill(2):
            continue
        #将这一行中的value读取出来
        for col in result_pd.columns:
            if col != 'timestamp':
                value = row[col]
                if int(value) == -1:
                    for i in range(row_count-1, -1, -1):
                        if result_pd.iloc[i][col] != -1:
                            value = result_pd.iloc[i][col]
                            break
                            
                #将这一行中的category读取出来
                category = col
                #将这一行中的数据添加到result_json中
                result_json.append({
                    "timestamp": timestamp,
                    "value": value,
                    "category": category
                })

    return ApiResponse(data=result_json, message="获取设备数据成功", code=200).to_json()