# analyze.py
from pyspark.sql import SparkSession
from pyspark.sql.types import StructType, StructField, StringType, DoubleType, LongType
from pyspark.sql.functions import *
from pyspark.ml.feature import PCA, StandardScaler, VectorAssembler
from pyspark.ml.stat import Correlation
from pyspark.ml.regression import LinearRegression
from pyspark.ml.evaluation import RegressionEvaluator
from pyspark.ml import Pipeline
import matplotlib.pyplot as plt
from matplotlib import rcParams
from matplotlib import font_manager
import numpy as np
import pandas as pd
import json
import pandas as pd
from datetime import datetime
from datetime import timedelta
from matplotlib import dates as mdates
from statsmodels.tsa.arima.model import ARIMA
from statsmodels.tsa.seasonal import seasonal_decompose
import seaborn as sns
import warnings

warnings.filterwarnings("ignore")
import os


# 在文件开头添加
PCA_OUTPUT_DIR = "/root/results/PCA"
os.makedirs(PCA_OUTPUT_DIR, exist_ok=True)
# 创建报表输出目录
REPORT_OUTPUT_DIR = "/root/results/Report"
os.makedirs(REPORT_OUTPUT_DIR, exist_ok=True)
PREDICT_OUTPUT_DIR = "/root/results/Predict"
os.makedirs(PREDICT_OUTPUT_DIR, exist_ok=True)
# 注册字体
try:
    font_manager.fontManager.addfont("/root/fonts/SimHei.ttf")
    plt.rcParams["font.family"] = "SimHei"
    plt.rcParams["axes.unicode_minus"] = False
    print("字体注册成功")
except Exception as e:
    print(f"字体注册失败: {str(e)}")


# Initialize Spark session
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


def get_device_names(spark: SparkSession):
    """动态获取/tmp/delta_tables/下的所有设备名（文件夹名）"""
    fs = spark._jvm.org.apache.hadoop.fs.FileSystem.get(
        spark._jsc.hadoopConfiguration()
    )
    delta_path = spark._jvm.org.apache.hadoop.fs.Path("/tmp/delta_tables/")
    devices = []

    if fs.exists(delta_path):
        for status in fs.listStatus(delta_path):
            if status.isDirectory():
                devices.append(status.getPath().getName())
    return devices


# PCA Analysis Functions
def perform_pca_analysis(
    spark: SparkSession, device_name: str, num_components: int = 3
):
    """
    Perform Principal Component Analysis on device data
    """
    table_path = f"/tmp/delta_tables/{device_name}"
    df = spark.read.format("delta").load(table_path)

    numeric_cols = [
        f.name for f in df.schema if isinstance(f.dataType, (DoubleType, LongType))
    ]

    # 新增检查：列数不足时跳过
    if len(numeric_cols) <= num_components:
        print(
            f"Skipping {device_name}: Only {len(numeric_cols)} numeric columns (need > {num_components})"
        )
        return None

    assembler = VectorAssembler(inputCols=numeric_cols, outputCol="features")
    assembled_data = assembler.transform(df)

    scaler = StandardScaler(
        inputCol="features", outputCol="scaled_features", withStd=True, withMean=True
    )
    scaler_model = scaler.fit(assembled_data)
    scaled_data = scaler_model.transform(assembled_data)

    corr_matrix = Correlation.corr(scaled_data, "scaled_features").collect()[0][0]

    pca = PCA(k=num_components, inputCol="scaled_features", outputCol="pca_features")
    pca_model = pca.fit(scaled_data)

    save_pca_results(device_name, pca_model, corr_matrix, numeric_cols, PCA_OUTPUT_DIR)

    # 生成热力图并保存
    plt.figure(figsize=(10, 6))
    sns.heatmap(
        pd.DataFrame(
            pca_model.pc.toArray(),
            columns=[f"PC{i+1}" for i in range(pca_model.getK())],  # 使用getK()获取的值
            index=numeric_cols,
        ),
        annot=True,
        cmap="coolwarm",
        center=0,
    )
    plt.title(f"{device_name} PCA载荷矩阵")
    plt.savefig(
        f"{PCA_OUTPUT_DIR}/{device_name}_components_heatmap.png", bbox_inches="tight"
    )
    plt.close()
    print(f"Plot saved to {PCA_OUTPUT_DIR}/{device_name}_components_heatmap.png")


def save_pca_results(
    device_name, model, corr_matrix, numeric_cols, output_dir
):
    """保存PCA结果到结构化文件"""
    os.makedirs(output_dir, exist_ok=True)

    # 获取实际的PCA组件数量
    num_components = model.getK()  # 使用getK()而不是直接访问k

    # 1. 保存相关矩阵
    pd.DataFrame(
        corr_matrix.toArray(), columns=numeric_cols, index=numeric_cols
    ).to_csv(f"{output_dir}/{device_name}_correlation.csv")

    # 2. 保存主成分信息
    components = pd.DataFrame(
        model.pc.toArray(),
        columns=[f"PC{i+1}" for i in range(num_components)],  # 使用getK()获取的值
        index=numeric_cols,
    )
    components.to_csv(f"{output_dir}/{device_name}_components.csv")

    # 3. 保存解释方差
    variance = {
        "explained_variance": model.explainedVariance.tolist(),
        "cumulative_variance": np.cumsum(model.explainedVariance).tolist(),
        "timestamp": datetime.now().isoformat(),
        "input_columns": numeric_cols,
        "num_components": num_components,  # 记录组件数量
    }
    with open(f"{output_dir}/{device_name}_variance.json", "w") as f:
        json.dump(variance, f, indent=2)

    # 4. 生成可视化报告
    report_html = f"""
    <html><body>
    <h1>{device_name} PCA报告</h1>
    <h2>累计解释方差: {np.sum(model.explainedVariance):.1%}</h2>
    <img src='{device_name}_components_heatmap.png' width=800>
    </body></html>
    """
    with open(f"{output_dir}/{device_name}_report.html", "w") as f:
        f.write(report_html)


# Reporting Functions
def generate_energy_reports(spark: SparkSession):
    """
    生成设备能源使用综合报表(日度、周度、月度)
    以HTML格式存储在本地文件系统，并增强可视化效果
    """

    # 获取所有设备名称并按类型分类
    devices = get_device_names(spark)
    device_categories = {
        "冷机": [d for d in devices if d.endswith("冷机")],
        "热机": [d for d in devices if d.endswith("燃烧机")],
        "三联供": [d for d in devices if d.endswith("号三联供")],
    }

    # 定义数据采集的月份范围
    target_months = ['2018-01', '2018-02', '2018-04', '2018-07', '2018-10']
    
    for category, device_list in device_categories.items():
        if not device_list:
            print(f"没有找到{category}设备")
            continue

        print(f"\n正在处理{category}设备: {device_list}")

        # 创建类别输出目录
        category_dir = f"{REPORT_OUTPUT_DIR}/{category}"
        os.makedirs(category_dir, exist_ok=True)

        # 读取所有同类设备数据并合并
        dfs = []
        for device in device_list:
            df = spark.read.format("delta").load(f"/tmp/delta_tables/{device}")
            df = df.withColumn("device_name", lit(device))
            df = df.withColumn(
                "timestamp", to_timestamp(col("timestamp"), "yyyy-MM-dd'T'HH:mm:ss'Z'")
            )
            # 筛选目标月份的数据
            df = df.filter(date_format(col("timestamp"), "yyyy-MM").isin(target_months))
            dfs.append(df)

        combined_df = dfs[0]
        for df in dfs[1:]:
            combined_df = combined_df.unionByName(df)

        # 根据设备类型定义关键指标和绘图函数
        if category == "冷机":
            # 定义关键指标
            target_cols = {
                "冷冻水出水温度": {"unit": "°C", "agg": ["max", "min", "avg"]},
                "冷冻水回水温度": {"unit": "°C", "agg": ["max", "min", "avg"]},
                "流量": {"unit": "m³", "agg": ["sum"]}
            }
            
            # 定义绘图函数
            def plot_cooling_device_data(pdf, time_col, time_period, output_dir):
                # 确保数据按时间排序
                pdf = pdf.sort_values(time_col)
                
                # 设置柱状图宽度
                bar_width = 0.5 if time_period == "daily" else 3 if time_period == "weekly" else 10
                
                # 1. 冷冻水出水温度的峰谷值、平均值
                plt.figure(figsize=(12, 6))
                plt.plot(pdf[time_col], pdf["max_冷冻水出水温度"], 'r-', label='峰值')
                plt.plot(pdf[time_col], pdf["min_冷冻水出水温度"], 'b-', label='谷值')
                plt.plot(pdf[time_col], pdf["avg_冷冻水出水温度"], 'g-', label='平均值')
                plt.title(f"冷机综合 冷冻水出水温度({time_period})")
                plt.ylabel("温度(°C)")
                plt.legend()
                plt.grid(True)
                plt.xticks(rotation=45)
                plt.tight_layout()
                plt.savefig(f"{output_dir}/冷机综合_{time_period}_outlet_temp.png")
                plt.close()
                
                # 2. 冷冻水出水温度的峰谷值期时长 (转换为小时)
                if 'peak_hours_冷冻水出水温度' in pdf.columns:
                    plt.figure(figsize=(12, 6))
                    plt.bar(pdf[time_col], pdf["peak_hours_冷冻水出水温度"] * 5/60, 
                           width=bar_width, color='r', alpha=0.6, label='峰值时长')
                    plt.bar(pdf[time_col], pdf["valley_hours_冷冻水出水温度"] * 5/60, 
                           width=bar_width, color='b', alpha=0.6, label='谷值时长', 
                           bottom=pdf["peak_hours_冷冻水出水温度"] * 5/60)
                    plt.title(f"冷机综合 冷冻水出水温度峰谷时长({time_period})")
                    plt.ylabel("小时数")
                    plt.legend()
                    plt.grid(True)
                    plt.xticks(rotation=45)
                    plt.tight_layout()
                    plt.savefig(f"{output_dir}/冷机综合_{time_period}_outlet_duration.png")
                    plt.close()
                
                # 3. 冷冻水回水温度的峰谷值、平均值
                plt.figure(figsize=(12, 6))
                plt.plot(pdf[time_col], pdf["max_冷冻水回水温度"], 'r-', label='峰值')
                plt.plot(pdf[time_col], pdf["min_冷冻水回水温度"], 'b-', label='谷值')
                plt.plot(pdf[time_col], pdf["avg_冷冻水回水温度"], 'g-', label='平均值')
                plt.title(f"冷机综合 冷冻水回水温度({time_period})")
                plt.ylabel("温度(°C)")
                plt.legend()
                plt.grid(True)
                plt.xticks(rotation=45)
                plt.tight_layout()
                plt.savefig(f"{output_dir}/冷机综合_{time_period}_inlet_temp.png")
                plt.close()
                
                # 4. 冷冻水回水温度的峰谷值期时长 (转换为小时)
                if 'peak_hours_冷冻水回水温度' in pdf.columns:
                    plt.figure(figsize=(12, 6))
                    plt.bar(pdf[time_col], pdf["peak_hours_冷冻水回水温度"] * 5/60, 
                           width=bar_width, color='r', alpha=0.6, label='峰值时长')
                    plt.bar(pdf[time_col], pdf["valley_hours_冷冻水回水温度"] * 5/60, 
                           width=bar_width, color='b', alpha=0.6, label='谷值时长',
                           bottom=pdf["peak_hours_冷冻水回水温度"] * 5/60)
                    plt.title(f"冷机综合 冷冻水回水温度峰谷时长({time_period})")
                    plt.ylabel("小时数")
                    plt.legend()
                    plt.grid(True)
                    plt.xticks(rotation=45)
                    plt.tight_layout()
                    plt.savefig(f"{output_dir}/冷机综合_{time_period}_inlet_duration.png")
                    plt.close()
                
                # 5. 总流量图
                plt.figure(figsize=(12, 6))
                plt.bar(pdf[time_col], pdf["sum_流量"], width=bar_width, color='g', alpha=0.6)
                plt.title(f"冷机综合 总流量({time_period})")
                plt.ylabel("流量(m³)")
                plt.grid(True)
                plt.xticks(rotation=45)
                plt.tight_layout()
                plt.savefig(f"{output_dir}/冷机综合_{time_period}_flow.png")
                plt.close()
                
                return [
                    f"冷机综合_{time_period}_outlet_temp.png",
                    f"冷机综合_{time_period}_outlet_duration.png",
                    f"冷机综合_{time_period}_inlet_temp.png",
                    f"冷机综合_{time_period}_inlet_duration.png",
                    f"冷机综合_{time_period}_flow.png"
                ]

        elif category == "热机":
            # 定义关键指标
            target_cols = {
                "负荷": {"unit": "kW", "agg": ["max", "min", "avg"]}
            }
            
            # 定义绘图函数
            def plot_heating_device_data(pdf, time_col, time_period, output_dir):
                # 确保数据按时间排序
                pdf = pdf.sort_values(time_col)
                
                # 设置柱状图宽度
                bar_width = 0.5 if time_period == "daily" else 3 if time_period == "weekly" else 10
                
                # 1. 负荷的峰谷值、平均值
                plt.figure(figsize=(12, 6))
                plt.plot(pdf[time_col], pdf["max_负荷"], 'r-', label='峰值')
                plt.plot(pdf[time_col], pdf["min_负荷"], 'b-', label='谷值')
                plt.plot(pdf[time_col], pdf["avg_负荷"], 'g-', label='平均值')
                plt.title(f"热机综合 负荷({time_period})")
                plt.ylabel("负荷(kW)")
                plt.legend()
                plt.grid(True)
                plt.xticks(rotation=45)
                plt.tight_layout()
                plt.savefig(f"{output_dir}/热机综合_{time_period}_load.png")
                plt.close()
                
                # 2. 负荷的峰谷值期时长 (转换为小时)
                if 'peak_hours_负荷' in pdf.columns:
                    plt.figure(figsize=(12, 6))
                    plt.bar(pdf[time_col], pdf["peak_hours_负荷"] * 5/60, 
                           width=bar_width, color='r', alpha=0.6, label='峰值时长')
                    plt.bar(pdf[time_col], pdf["valley_hours_负荷"] * 5/60, 
                           width=bar_width, color='b', alpha=0.6, label='谷值时长',
                           bottom=pdf["peak_hours_负荷"] * 5/60)
                    plt.title(f"热机综合 负荷峰谷时长({time_period})")
                    plt.ylabel("小时数")
                    plt.legend()
                    plt.grid(True)
                    plt.xticks(rotation=45)
                    plt.tight_layout()
                    plt.savefig(f"{output_dir}/热机综合_{time_period}_load_duration.png")
                    plt.close()
                
                return [
                    f"热机综合_{time_period}_load.png",
                    f"热机综合_{time_period}_load_duration.png"
                ]

        elif category == "三联供":
            # 定义关键指标
            target_cols = {
                "热水回水温度": {"unit": "°C", "agg": ["max", "min", "avg"]},
                "热水出水温度": {"unit": "°C", "agg": ["max", "min", "avg"]},
                "冷冻水回水温度": {"unit": "°C", "agg": ["max", "min", "avg"]},
                "冷冻水出水温度": {"unit": "°C", "agg": ["max", "min", "avg"]}
            }
            
            # 定义绘图函数
            def plot_cchp_device_data(pdf, time_col, time_period, output_dir):
                # 确保数据按时间排序
                pdf = pdf.sort_values(time_col)
                
                # 设置柱状图宽度
                bar_width = 0.5 if time_period == "daily" else 3 if time_period == "weekly" else 10
                
                image_files = []
                
                # 对每个温度字段生成两张图
                for temp_col in ["热水回水温度", "热水出水温度", "冷冻水回水温度", "冷冻水出水温度"]:
                    # 1. 温度峰谷值、平均值
                    plt.figure(figsize=(12, 6))
                    plt.plot(pdf[time_col], pdf[f"max_{temp_col}"], 'r-', label='峰值')
                    plt.plot(pdf[time_col], pdf[f"min_{temp_col}"], 'b-', label='谷值')
                    plt.plot(pdf[time_col], pdf[f"avg_{temp_col}"], 'g-', label='平均值')
                    plt.title(f"三联供综合 {temp_col}({time_period})")
                    plt.ylabel("温度(°C)")
                    plt.legend()
                    plt.grid(True)
                    plt.xticks(rotation=45)
                    plt.tight_layout()
                    temp_img = f"三联供综合_{time_period}_{temp_col.replace(' ', '')}.png"
                    plt.savefig(f"{output_dir}/{temp_img}")
                    plt.close()
                    image_files.append(temp_img)
                    
                    # 2. 温度峰谷值期时长 (转换为小时)
                    peak_col = f"peak_hours_{temp_col}"
                    if peak_col in pdf.columns:
                        plt.figure(figsize=(12, 6))
                        plt.bar(pdf[time_col], pdf[peak_col] * 5/60, 
                               width=bar_width, color='r', alpha=0.6, label='峰值时长')
                        plt.bar(pdf[time_col], pdf[f"valley_hours_{temp_col}"] * 5/60, 
                               width=bar_width, color='b', alpha=0.6, label='谷值时长',
                               bottom=pdf[peak_col] * 5/60)
                        plt.title(f"三联供综合 {temp_col}峰谷时长({time_period})")
                        plt.ylabel("小时数")
                        plt.legend()
                        plt.grid(True)
                        plt.xticks(rotation=45)
                        plt.tight_layout()
                        duration_img = f"三联供综合_{time_period}_{temp_col.replace(' ', '')}_duration.png"
                        plt.savefig(f"{output_dir}/{duration_img}")
                        plt.close()
                        image_files.append(duration_img)
                
                return image_files

        # 生成报表数据
        def generate_report(time_period):
            time_col = {"daily": "day", "weekly": "week", "monthly": "month"}[time_period]
            
            # 基础统计
            agg_exprs = []
            for col_name, col_info in target_cols.items():
                for agg_func in col_info["agg"]:
                    if agg_func == "sum":
                        agg_exprs.append(sum(col(col_name)).alias(f"{agg_func}_{col_name}"))
                    elif agg_func == "avg":
                        agg_exprs.append(avg(col(col_name)).alias(f"{agg_func}_{col_name}"))
                    elif agg_func == "max":
                        agg_exprs.append(max(col(col_name)).alias(f"{agg_func}_{col_name}"))
                    elif agg_func == "min":
                        agg_exprs.append(min(col(col_name)).alias(f"{agg_func}_{col_name}"))
            
            report_df = (
                combined_df.groupBy(
                    date_trunc(time_col, "timestamp").alias(time_col)
                )
                .agg(*agg_exprs, count("*").alias("record_count"))
                .orderBy(time_col)
            )

            # 峰值统计 (基于5分钟间隔数据)
            for col_name in target_cols.keys():
                peak_stats = (
                    combined_df.withColumn(
                        f"peak_flag_{col_name}",
                        when(
                            col(col_name) >= 0.8 * combined_df.select(max(col_name)).first()[0],
                            1
                        ).otherwise(0),
                    )
                    .withColumn(
                        f"valley_flag_{col_name}",
                        when(
                            col(col_name) <= 0.2 * combined_df.select(min(col_name)).first()[0]
                            + 0.8 * combined_df.select(min(col_name)).first()[0],
                            1
                        ).otherwise(0),
                    )
                    .groupBy(
                        date_trunc(time_col, "timestamp").alias(time_col)
                    )
                    .agg(
                        sum(f"peak_flag_{col_name}").alias(f"peak_hours_{col_name}"),
                        sum(f"valley_flag_{col_name}").alias(f"valley_hours_{col_name}"),
                    )
                )
                report_df = report_df.join(peak_stats, [time_col])

            return report_df

        # 生成并保存三种时间粒度的报表
        for time_period in ["daily", "weekly", "monthly"]:
            report_df = generate_report(time_period)

            # 转换 Spark 的日期列为字符串，避免 toPandas() 报错
            time_col = {"daily": "day", "weekly": "week", "monthly": "month"}[time_period]
            report_df = report_df.withColumn(time_col, col(time_col).cast("string"))

            # 转换为Pandas DataFrame时处理日期列
            pdf = report_df.toPandas()
            pdf[time_col] = pd.to_datetime(pdf[time_col]).astype("datetime64[ns]")
            
            # 确保表格数据也按时间排序
            pdf = pdf.sort_values(time_col)

            # 生成图表
            if category == "冷机":
                images = plot_cooling_device_data(pdf, time_col, time_period, category_dir)
            elif category == "热机":
                images = plot_heating_device_data(pdf, time_col, time_period, category_dir)
            elif category == "三联供":
                images = plot_cchp_device_data(pdf, time_col, time_period, category_dir)

            # 生成HTML报表
            html_content = f"""
            <html>
            <head>
                <title>{category} {time_period}报表</title>
                <style>
                    body {{ font-family: SimHei, sans-serif; margin: 20px; }}
                    h1 {{ color: #2e6c80; }}
                    h2 {{ color: #3e7c90; }}
                    .report-container {{ max-width: 1200px; margin: 0 auto; }}
                    .image-grid {{
                        display: grid;
                        grid-template-columns: repeat(2, 1fr);
                        gap: 20px;
                        margin-bottom: 30px;
                    }}
                    .image-container {{
                        border: 1px solid #ddd;
                        padding: 10px;
                        background: #f9f9f9;
                        text-align: center;
                    }}
                    .image-container img {{
                        max-width: 100%;
                        height: auto;
                    }}
                    .image-title {{
                        font-weight: bold;
                        margin: 10px 0;
                    }}
                    table {{
                        border-collapse: collapse;
                        width: 100%;
                        margin-top: 30px;
                    }}
                    th, td {{
                        border: 1px solid #ddd;
                        padding: 8px;
                        text-align: left;
                    }}
                    th {{ background-color: #f2f2f2; }}
                    tr:nth-child(even) {{ background-color: #f9f9f9; }}
                    .header {{
                        background-color: #2e6c80;
                        color: white;
                        padding: 15px;
                        text-align: center;
                        margin-bottom: 20px;
                    }}
                    .timestamp {{
                        text-align: right;
                        color: #666;
                        font-style: italic;
                        margin-bottom: 20px;
                    }}
                    .summary {{
                        background-color: #f0f8ff;
                        padding: 15px;
                        border-radius: 5px;
                        margin-bottom: 20px;
                    }}
                </style>
            </head>
            <body>
                <div class="report-container">
                    <div class="header">
                        <h1>{category}综合 {time_period}能源报表</h1>
                    </div>
                    
                    <div class="timestamp">
                        生成时间: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')} | 
                        数据月份: {', '.join(target_months)}
                    </div>
                    
                    <div class="summary">
                        <h3>设备列表: {', '.join(device_list)}</h3>
                        <p>数据采集间隔: 5分钟 | 峰谷时长计算: 数据点数 × 5分钟</p>
                    </div>
                    
                    <h2>数据可视化</h2>
                    <div class="image-grid">
            """

            # 添加所有图片到HTML
            for img in images:
                img_name = img.replace(f"{time_period}_", "").replace(".png", "").replace("_", " ")
                html_content += f"""
                        <div class="image-container">
                            <div class="image-title">{img_name}</div>
                            <img src="{img}" alt="{img_name}">
                        </div>
                """

            html_content += f"""
                    </div>
                    
                    <h2>详细数据</h2>
                    {pdf.to_html(index=False)}
                </div>
            </body>
            </html>
            """

            # 保存HTML文件
            html_file = f"{category_dir}/{category}综合_{time_period}_report.html"
            with open(html_file, "w", encoding="utf-8") as f:
                f.write(html_content)

            print(f"已生成 {category}综合 {time_period} 报表: {html_file}")

# Forecasting Functions
def prepare_time_series_data_minutes(
    spark: SparkSession, 
    device_name: str, 
    target_col: str, 
    train_window_minutes: int = 300,
    forecast_minutes: int = 180,
    forecast_datetime: str = "2018-04-10 03:00:00"
):
    """
    Prepare time series data for minute-level forecasting
    
    Args:
        spark: SparkSession
        device_name: Name of the device (should be "2号站三联供")
        target_col: Target column to forecast ("热总能量计累计热量" or "总冷累计能量（kWh）")
        train_window_minutes: Number of minutes to use as training window
        forecast_minutes: Number of minutes to forecast
        forecast_datetime: Datetime to forecast from (format "YYYY-MM-DD HH:MM:SS")
    
    Returns:
        tuple: (training_series, test_series, forecast_start_datetime)
    """
    table_path = f"/tmp/delta_tables/{device_name}"
    df = spark.read.format("delta").load(table_path)
    
    # Convert timestamp and filter data
    df = df.withColumn("timestamp", to_timestamp(col("timestamp"), "yyyy-MM-dd'T'HH:mm:ss'Z'"))
    
    # Convert forecast_datetime to datetime
    forecast_dt = datetime.strptime(forecast_datetime, "%Y-%m-%d %H:%M:%S")
    window_start = forecast_dt - timedelta(minutes=train_window_minutes)
    forecast_end = forecast_dt + timedelta(minutes=forecast_minutes)
    
    # Filter data for the selected time window
    training_data = df.filter(
        (col("timestamp") >= window_start) & 
        (col("timestamp") < forecast_dt)
    )
    
    # Get test data (forecast period)
    test_data = df.filter(
        (col("timestamp") >= forecast_dt) & 
        (col("timestamp") < forecast_end)
    )
    
    # 转换为Pandas前先处理时间戳列
    training_data = training_data.withColumn("timestamp", col("timestamp").cast("string"))
    test_data = test_data.withColumn("timestamp", col("timestamp").cast("string"))
    
    # Convert to pandas
    train_pdf = training_data.select("timestamp", target_col).toPandas()
    test_pdf = test_data.select("timestamp", target_col).toPandas()
    
    # 处理时间戳列转换
    train_pdf["timestamp"] = pd.to_datetime(train_pdf["timestamp"]).astype("datetime64[ns]")
    test_pdf["timestamp"] = pd.to_datetime(test_pdf["timestamp"]).astype("datetime64[ns]")
    
    # Set index and sort
    train_series = train_pdf.set_index("timestamp")[target_col].sort_index()
    test_series = test_pdf.set_index("timestamp")[target_col].sort_index()
    
    print(f"训练数据点数: {len(train_series)} ({(train_series.index[-1] - train_series.index[0]).total_seconds()/60:.1f} 分钟)")
    print(f"测试数据点数: {len(test_series)} ({(test_series.index[-1] - test_series.index[0]).total_seconds()/60:.1f} 分钟)")
    
    return train_series, test_series, forecast_dt

def arima_forecast_minutes(series, steps: int = 36, order: tuple = (2, 1, 2)):
    """
    Minute-level ARIMA forecasting
    
    Args:
        series: Training time series
        steps: Number of 5-minute intervals to forecast
        order: ARIMA order parameters
        
    Returns:
        pd.Series: Forecasted values with datetime index
    """
    try:
        # 检查数据频率是否为5分钟
        freq_minutes = series.index.to_series().diff().median().total_seconds() / 60
        print(f"数据频率: {freq_minutes} 分钟")
        
        model = ARIMA(series, order=order)
        model_fit = model.fit()
        forecast = model_fit.forecast(steps=steps)
        
        # 生成预测时间索引
        last_time = series.index[-1]
        freq = series.index.to_series().diff().median()
        forecast_index = pd.date_range(
            start=last_time + freq, 
            periods=steps, 
            freq=freq
        )
        
        print(f"成功生成 {len(forecast)} 个预测点 ({(forecast_index[-1] - forecast_index[0]).total_seconds()/60:.1f} 分钟)")
        return pd.Series(forecast.values, index=forecast_index)
        
    except Exception as e:
        print(f"ARIMA 模型错误: {str(e)}")
        return pd.Series()  # 返回空序列

def plot_minute_forecast_results(
    train_series: pd.Series,
    test_series: pd.Series,
    forecast_series: pd.Series,
    target_name: str,
    forecast_datetime: str,
    train_window_minutes: int,
    forecast_minutes: int,
    output_dir: str = PREDICT_OUTPUT_DIR
):
    """
    Plot minute-level forecast results and generate HTML report
    
    Args:
        train_series: Training data series
        test_series: Test data series
        forecast_series: Forecasted data series
        target_name: Name of target variable
        forecast_datetime: Datetime being forecasted
        train_window_minutes: Training window size in minutes
        forecast_minutes: Forecast duration in minutes
        output_dir: Directory to save plots and HTML
    """
    os.makedirs(output_dir, exist_ok=True)
     
    # Calculate average predicted and actual values for comparison
    avg_pred = forecast_series.mean()
    avg_actual = test_series.mean() if not test_series.empty else None
    
    # Determine operational suggestion
    if avg_actual is not None:
        if avg_pred < avg_actual:
            suggestion = "机组操作建议为增加启动设备数量"
            suggestion_color = "red"
        else:
            suggestion = "机组操作建议为减少启动设备数量"
            suggestion_color = "green"
    else:
        suggestion = "无实际数据可用于生成操作建议"
        suggestion_color = "gray"
    
    # 1. 生成预测图
    plt.figure(figsize=(15, 8))
    
    # Plot training data
    plt.plot(
        train_series.index, 
        train_series.values, 
        'b-', 
        linewidth=2,
        label=f'Training Data ({train_window_minutes} 分钟)'
    )
    
    # Plot test data
    plt.plot(
        test_series.index, 
        test_series.values, 
        'g-', 
        linewidth=2,
        label='Actual Values'
    )

    # Plot forecast
    plt.plot(
        forecast_series.index, 
        forecast_series.values, 
        'r--', 
        linewidth=2,
        label=f'Forecast ({forecast_minutes} 分钟)'
    )
    
    # Add vertical line at forecast start
    forecast_start = forecast_series.index[0]
    plt.axvline(
        x=forecast_start, 
        color='k', 
        linestyle='--', 
        alpha=0.5,
        linewidth=2
    )
    
    # Add shaded area for training window
    plt.axvspan(
        train_series.index[0], 
        forecast_start, 
        color='blue', 
        alpha=0.05,
        label='训练窗口'
    )
    
    plt.title(
        f"{target_name} 分钟级预测\n"
        f"预测时间: {forecast_datetime} | "
        f"训练窗口: {train_window_minutes} 分钟 | "
        f"预测时长: {forecast_minutes} 分钟"
    )
    plt.ylabel(target_name)
    plt.xlabel("时间")
    plt.legend()
    plt.grid(True)
    
    # Format x-axis
    plt.gca().xaxis.set_major_formatter(mdates.DateFormatter('%m-%d %H:%M'))
    plt.gca().xaxis.set_major_locator(mdates.AutoDateLocator())
    plt.xticks(rotation=45)
    
    # Save plot
    plot_filename = (
        f"{target_name.replace(' ', '_')}_minute_forecast_"
        f"{forecast_datetime.replace(' ', '_').replace(':', '')}_"
        f"train{train_window_minutes}_pred{forecast_minutes}.png"
    )
    plot_path = os.path.join(output_dir, plot_filename)
    plt.savefig(plot_path, bbox_inches="tight", dpi=120)
    plt.close()
    
    # 2. 生成误差分析图
    if not test_series.empty and not forecast_series.empty:
        # 对齐测试数据和预测数据
        common_index = test_series.index.intersection(forecast_series.index)
        if len(common_index) > 0:
            aligned_test = test_series[common_index]
            aligned_forecast = forecast_series[common_index]
            
            # 计算误差
            errors = aligned_test - aligned_forecast
            mae = errors.abs().mean()
            mape = (errors.abs() / aligned_test).mean() * 100
            
            # 绘制误差图
            plt.figure(figsize=(15, 6))
            plt.plot(errors.index, errors.values, 'm-', label='预测误差')
            plt.axhline(y=0, color='k', linestyle='--', alpha=0.5)
            plt.title(f"{target_name} 预测误差\nMAE: {mae:.2f} | MAPE: {mape:.2f}%")
            plt.ylabel("误差值")
            plt.xlabel("时间")
            plt.legend()
            plt.grid(True)
            
            # 格式化x轴
            plt.gca().xaxis.set_major_formatter(mdates.DateFormatter('%m-%d %H:%M'))
            plt.gca().xaxis.set_major_locator(mdates.AutoDateLocator())
            plt.xticks(rotation=45)
            
            # 保存误差图
            error_plot_filename = plot_filename.replace("forecast", "error")
            error_plot_path = os.path.join(output_dir, error_plot_filename)
            plt.savefig(error_plot_path, bbox_inches="tight", dpi=120)
            plt.close()
        else:
            error_plot_filename = None
            mae = None
            mape = None
    else:
        error_plot_filename = None
        mae = None
        mape = None
    
    # 3. 生成HTML报告
    html_content = f"""
    <html>
    <head>
        <title>{target_name} 预测报告</title>
        <style>
            body {{ 
                font-family: SimHei, sans-serif; 
                margin: 20px;
                color: #333;
            }}
            .header {{
                background-color: #2e6c80;
                color: white;
                padding: 20px;
                text-align: center;
                margin-bottom: 20px;
                border-radius: 5px;
            }}
            .container {{
                max-width: 1200px;
                margin: 0 auto;
            }}
            .summary-card {{
                background-color: #f8f9fa;
                border-radius: 5px;
                padding: 15px;
                margin-bottom: 20px;
                box-shadow: 0 2px 5px rgba(0,0,0,0.1);
            }}
            .image-container {{
                margin: 20px 0;
                text-align: center;
            }}
            .image-container img {{
                max-width: 100%;
                border: 1px solid #ddd;
                border-radius: 4px;
                padding: 5px;
                background: white;
                box-shadow: 0 2px 5px rgba(0,0,0,0.1);
            }}
            .image-title {{
                font-weight: bold;
                margin: 10px 0;
                color: #2e6c80;
            }}
            .data-table {{
                width: 100%;
                border-collapse: collapse;
                margin-top: 20px;
            }}
            .data-table th, .data-table td {{
                border: 1px solid #ddd;
                padding: 8px;
                text-align: left;
            }}
            .data-table th {{
                background-color: #2e6c80;
                color: white;
            }}
            .data-table tr:nth-child(even) {{
                background-color: #f2f2f2;
            }}
            .metrics {{
                display: flex;
                justify-content: space-around;
                margin: 20px 0;
            }}
            .metric-card {{
                background: white;
                border-radius: 5px;
                padding: 15px;
                width: 30%;
                text-align: center;
                box-shadow: 0 2px 5px rgba(0,0,0,0.1);
            }}
            .metric-value {{
                font-size: 24px;
                font-weight: bold;
                color: #2e6c80;
            }}
            .metric-label {{
                color: #666;
                font-size: 14px;
            }}
            .timestamp {{
                text-align: right;
                color: #666;
                font-style: italic;
                margin-bottom: 20px;
            }}
        </style>
    </head>
    <body>
        <div class="container">
            <div class="header">
                <h1>{target_name} 预测报告</h1>
            </div>
            
            <div class="timestamp">
                生成时间: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}
            </div>
            
            <div class="summary-card">
                <h3>预测配置</h3>
                <p><strong>预测时间点:</strong> {forecast_datetime}</p>
                <p><strong>训练窗口:</strong> {train_window_minutes} 分钟 ({len(train_series)} 个数据点)</p>
                <p><strong>预测时长:</strong> {forecast_minutes} 分钟 ({len(forecast_series)} 个预测点)</p>
                <p><strong>实际数据点数:</strong> {len(test_series)} 个</p>
            </div>
            
            <div class="metrics">
                <div class="metric-card">
                    <div class="metric-value">{len(forecast_series)}</div>
                    <div class="metric-label">预测点数</div>
                </div>
    """
    
    # 添加误差指标
    if mae is not None and mape is not None:
        html_content += f"""
                <div class="metric-card">
                    <div class="metric-value">{mae:.2f}</div>
                    <div class="metric-label">平均绝对误差(MAE)</div>
                </div>
                <div class="metric-card">
                    <div class="metric-value">{mape:.2f}%</div>
                    <div class="metric-label">平均绝对百分比误差(MAPE)</div>
                </div>
        """
    else:
        html_content += """
                <div class="metric-card">
                    <div class="metric-value">N/A</div>
                    <div class="metric-label">误差指标不可用</div>
                </div>
        """
    
    html_content += f"""
            </div>
            
            <div class="image-container">
                <div class="image-title">预测结果对比</div>
                <img src="{plot_filename}" alt="预测结果图">
            </div>
    """
    
    # 添加误差图
    if error_plot_filename:
        html_content += f"""
            <div class="image-container">
                <div class="image-title">预测误差分析</div>
                <img src="{error_plot_filename}" alt="预测误差图">
            </div>
        """
    
    # 添加数据表格
    html_content += f"""
            <h3>预测数据详情</h3>
            <table class="data-table">
                <tr>
                    <th>时间</th>
                    <th>预测值</th>
                    <th>实际值</th>
                    <th>误差</th>
                </tr>
    """
    
    # 添加表格行
    if not test_series.empty and not forecast_series.empty:
        common_index = test_series.index.intersection(forecast_series.index)
        for idx in common_index[:20]:  # 只显示前20行
            actual = test_series[idx]
            pred = forecast_series[idx]
            error = actual - pred
            html_content += f"""
                <tr>
                    <td>{idx.strftime('%m-%d %H:%M')}</td>
                    <td>{pred:.2f}</td>
                    <td>{actual:.2f}</td>
                    <td style="color: {'red' if error > 0 else 'green'}">{error:.2f}</td>
                </tr>
            """
    else:
        html_content += """
                <tr>
                    <td colspan="4" style="text-align: center;">无实际数据可用于比较</td>
                </tr>
        """
    
    html_content += f"""
            </table>
            
            <div style="margin-top: 30px; padding: 10px; background: #f8f9fa; border-radius: 5px;">
                <p><strong>说明:</strong> 本报告基于ARIMA模型生成，预测间隔为5分钟。</p>
                <p>训练窗口: 预测点前{train_window_minutes}分钟的数据用于训练模型。</p>
                <p>误差计算: 仅当预测时间段有实际数据时才会计算误差指标。</p>
            </div>
            <div style="margin-top: 20px; padding: 15px; background-color: #f0f8ff; border-radius: 5px; border-left: 5px solid">
                <h3>操作建议</h3>
                <p style="color: {suggestion_color}; font-weight: bold;">{suggestion}</p>
            </div>
        </div>
    </body>
    </html>
    """.format(train_window_minutes)
    
    # 保存HTML文件
    html_filename = plot_filename.replace(".png", ".html")
    html_path = os.path.join(output_dir, html_filename)
    with open(html_path, "w", encoding="utf-8") as f:
        f.write(html_content)
    
    print(f"预测报告已保存到: {html_path}")
    return plot_path, error_plot_filename if error_plot_filename else None

def predict_energy_trend(spark: SparkSession):
    """
    Main function to predict energy trends at minute level
    
    Args:
        spark: SparkSession
    """
    # 配置参数
    device_name = "2号站三联供"
    target_columns = ["热总能量计累计热量", "总冷累计能量（kWh）"]
    
    # 预测配置 - 可以修改这些参数
    forecast_datetime = "2018-04-10 03:00:00"  # 预测起始时间
    train_window_minutes = 600  # 训练窗口(分钟)
    forecast_minutes = 180      # 预测时长(分钟)
    
    print(f"\nPredicting minute-level energy trends for {device_name}...")
    print(f"Forecast datetime: {forecast_datetime}")
    print(f"Training window: {train_window_minutes} minutes")
    print(f"Forecast duration: {forecast_minutes} minutes")
    
    for target_col in target_columns:
        print(f"\nProcessing {target_col}...")
        
        try:
            # 准备数据
            train_series, test_series, forecast_dt = prepare_time_series_data_minutes(
                spark, 
                device_name, 
                target_col, 
                train_window_minutes,
                forecast_minutes,
                forecast_datetime
            )
            
            # 检查是否有足够数据
            if len(train_series) < 10:
                print(f"训练数据不足 ({len(train_series)} 个点)")
                continue
                
            # 计算需要预测的步数 (假设数据是5分钟间隔)
            steps = int(forecast_minutes / 5)
            print(f"预测步数: {steps} (5分钟间隔)")
            
            # 进行预测
            forecast_series = arima_forecast_minutes(train_series, steps=steps)
            
            if len(forecast_series) == 0:
                print("预测失败")
                continue
                
            # 绘制结果
            plot_minute_forecast_results(
                train_series,
                test_series,
                forecast_series,
                target_col,
                forecast_datetime,
                train_window_minutes,
                forecast_minutes
            )
            
        except Exception as e:
            print(f"处理 {target_col} 时出错: {str(e)}")
            continue

# 修改主函数
def run_analysis():
    spark = init_spark()

    print("\nRunning PCA Analysis...")
    for device_name in get_device_names(spark):  # 改为动态获取
        print(f"Analyzing device: {device_name}")
        perform_pca_analysis(spark, device_name)

    print("\nGenerating Energy Reports...")
    generate_energy_reports(spark)

    print("\nPredicting Energy Trends...")
    # 新增分钟级预测
    predict_energy_trend(spark)

    print("\nAnalysis completed!")
    
if __name__ == "__main__":
    run_analysis()