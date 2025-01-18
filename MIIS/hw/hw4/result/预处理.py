import pandas as pd
from pypinyin import pinyin, lazy_pinyin, Style

# 读取 Excel 文件
df = pd.read_excel("data.xlsx")
# 确保 START 和 END 列是 datetime 类型
df["ACTIVITY_START"] = pd.to_datetime(df["ACTIVITY_START"], format="%Y-%m-%d %H:%M:%S")
df["ACTIVITY_END"] = pd.to_datetime(df["ACTIVITY_END"], format="%Y-%m-%d %H:%M:%S")

# 计算 START 和 END 的平均值作为 TIME 列
df["TIME"] = df.apply(
    lambda row: (
        row["ACTIVITY_START"] + (row["ACTIVITY_END"] - row["ACTIVITY_START"]) / 2
    ).strftime("%Y-%m-%d %H:%M:%S"),
    axis=1,
)
saved_data_list = []
other_data_list = []


# 过滤掉所有“第一个事件不是挂号”的数据
def filter_by_activity(group: pd.DataFrame) -> pd.DataFrame:
    group = group.sort_values(by="ACTIVITY_START")
    first_activity = group.iloc[0]["ACTIVITY"]

    if first_activity != "挂号":
        other_data_list.append(group)
    else:
        saved_data_list.append(group)

    return group


# 把所有中文转换为拼音
def chinese_to_pinyin(group: pd.DataFrame) -> pd.DataFrame:
    group["ACTIVITY"] = group["ACTIVITY"].apply(
        lambda x: "".join(lazy_pinyin(x))
    )
    return group


# 处理并保存结果
df.groupby("GUAHAO_ID").apply(filter_by_activity)
# 合并并保存数据
saved_data = pd.concat(saved_data_list, ignore_index=True)
chinese_to_pinyin(saved_data)
saved_data.to_csv("processed_data.csv", index=False)
if len(other_data_list) > 0:
    other_data = pd.concat(other_data_list, ignore_index=True)
    other_data.to_csv("other_data.csv", index=False)

# 按 ACTIVITY 分组，并计算每组的平均 DURATION
result = df.groupby('ACTIVITY')['DURATION'].mean().reset_index()

result.to_csv("avg_duration.csv", index=False)