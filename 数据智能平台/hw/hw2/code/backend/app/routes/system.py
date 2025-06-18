from flask import Blueprint, request, jsonify
from app.services.spark_service import SparkService
from app.services.delta_table_service import DeltaTableService
from pyspark.sql import DataFrame
from app.services.mqtt_service import realtime_data
from app.utils.response import ApiResponse

# 创建 system 蓝图
bp = Blueprint('system', __name__)

@bp.route('/realtime_data', methods=['GET'])
def get_realtime_data():
    """
    获取实时数据
    """
    # 返回实时数据
    return ApiResponse(data=realtime_data, message="获取实时数据成功", code=200).to_json()