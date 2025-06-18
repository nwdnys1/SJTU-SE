from flask import Blueprint, request, jsonify
from app.services.spark_service import SparkService
from app.services.delta_table_service import DeltaTableService
from pyspark.sql import DataFrame

# 创建 predict 蓝图
bp = Blueprint('predict', __name__)