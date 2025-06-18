import pandas as pd
import os
from pandas import json_normalize


# 从 txt 文件读取数据（假设以空格或逗号分隔）
def txt_to_csv(txt_file, csv_file, sep=" "):
    # 读取 txt 文件
    df = pd.read_csv(
        txt_file, sep=sep, header=None
    )  # 不设置列名，假设每行数据是无标题的
    df.to_csv(csv_file, index=False, header=False)  # 保存为 CSV 文件


# 从嵌套的 json 文件读取数据并转换为 csv 文件
def json_to_csv(json_file, csv_file):
    # 读取 json 文件
    with open(json_file, "r") as file:
        data = pd.read_json(file)

    # 使用 json_normalize 展开嵌套 JSON 数据
    df = json_normalize(
        data["response"]["submissions"],
        record_path="data",  # 展开 data 列表
        meta=[
            "id",
            "timestamp",
            "user_agent",
            "remote_addr",
            "payment_status",
        ],  # 保留其他字段
        errors="ignore",
    )

    # 保存为 CSV 文件
    df.to_csv(csv_file, index=False)


# 从 csv 文件读取数据并转换为 json
def csv_to_json(csv_file, json_file):
    # 读取 csv 文件
    df = pd.read_csv(csv_file)
    df.to_json(
        json_file, orient="records", lines=True
    )  # 使用 records 格式保存为 json 文件


# 遍历“output”文件夹下的所有文件 根据文件后缀名调用相应的函数 保存到result文件夹下
import os

output_dir = "output"
result_dir = "result"
for file in os.listdir(output_dir):
    if file.endswith(".txt"):
        txt_to_csv(
            os.path.join(output_dir, file),
            os.path.join(result_dir, file[:-4] + ".csv"),
            "\t",
        )
    elif file.endswith(".json"):
        json_to_csv(
            os.path.join(output_dir, file), os.path.join(result_dir, file[:-5] + ".csv")
        )
    elif file.endswith(".csv"):
        csv_to_json(
            os.path.join(output_dir, file),
            os.path.join(result_dir, file[:-4] + ".json"),
        )
    else:
        print(f"Unsupported file format: {file}")
