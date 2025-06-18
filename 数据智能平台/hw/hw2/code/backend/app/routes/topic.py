from flask import Blueprint, request, jsonify
from app.services.spark_service import SparkService
from app.services.delta_table_service import DeltaTableService
from pyspark.sql import DataFrame
from app.utils.response import ApiResponse

# 创建 topic 蓝图
bp = Blueprint('topic', __name__)

# 获取所有设备和所有 Delta 表的所有列名
@bp.route('/all_topics', methods=['GET'])
def get_all_columns():
    base_path = "/tmp/delta_tables"  # HDFS 基础路径
    all_columns = DeltaTableService.get_all_delta_tables_columns(base_path)

    return ApiResponse(data=all_columns, message="获取所有设备和所有 Delta 表的所有列名成功", code=200).to_json()

# get_tables_by_column
@bp.route('/get_tables_by_column', methods=['GET'])
def get_tables_by_column():
    column_name = request.args.get('column_name')
    base_path = "/tmp/delta_tables"
    # 获取包含指定列名的 Delta 表路径
    table_names = DeltaTableService.get_tables_by_column(base_path, column_name)
    
    # 组装结果为包含 table_name 和 table_path 的字典列表
    result = [
        {
            "table_name": name,  # 假设表名是路径的最后一部分
        }
        for name in table_names
    ]

    return ApiResponse(data=result, message="获取包含指定列名的 Delta 表成功", code=200).to_json()