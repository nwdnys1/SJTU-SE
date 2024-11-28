import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# # 从 Excel 文件读取数据
# df_latency = pd.read_excel('./result.xlsx', sheet_name='Latency')
# df_throughput = pd.read_excel('./result.xlsx', sheet_name='Throughput')

# # 获取数据列名
# latency_metrics = df_latency.columns[1:]
# throughput_metrics = df_throughput.columns[1:]

# # 绘制 Latency 图
# plt.figure(figsize=(10, 6))
# datasize = df_latency['datasize']

# for metric in latency_metrics:
#     plt.plot(datasize, df_latency[metric], marker='o', label=metric, linewidth=3)  # 设置折线粗细为2

# plt.xlabel('Data Size')
# plt.ylabel('Latency (microseconds)')
# plt.title('Latency Metrics')
# plt.xticks(datasize)
# plt.legend()
# plt.grid(True)
# plt.show()

# # 绘制 Throughput 图
# plt.figure(figsize=(10, 6))

# for metric in throughput_metrics:
#     plt.plot(datasize, df_throughput[metric], marker='o', label=metric, linewidth=3)  # 设置折线粗细为2

# plt.xlabel('Data Size')
# plt.ylabel('Throughput (operations per second)')
# plt.title('Throughput Metrics')
# plt.xticks(datasize)
# plt.legend()
# plt.grid(True)
# plt.show()

# # 读取 Bloomfilter 表的数据
# df_bloomfilter = pd.read_excel('./result.xlsx', sheet_name='Bloomfilter')

# # 提取数据
# cachepolicy = df_bloomfilter.iloc[:, 0]
# get_latency = df_bloomfilter.iloc[:, 1]
# get_throughput = df_bloomfilter.iloc[:, 2]

# # 设置每个柱状图的宽度
# bar_width = 0.35

# # 创建图形和轴对象
# fig, ax1 = plt.subplots(figsize=(10, 6))

# # 绘制时延柱状图
# ax1.bar(np.arange(len(cachepolicy)), get_latency, bar_width, color='red', alpha=0.7, label='Get Latency')
# ax1.set_xlabel('Cache Policy')
# ax1.set_ylabel('Get Latency', color='red')

# # 设置左侧轴的刻度范围
# ax1.set_ylim([0, max(get_latency) * 1.1])

# # 创建右侧轴对象
# ax2 = ax1.twinx()

# # 绘制吞吐量柱状图
# ax2.bar(np.arange(len(cachepolicy)) + bar_width, get_throughput, bar_width, color='blue', alpha=0.7, label='Get Throughput')
# ax2.set_ylabel('Get Throughput', color='blue')

# # 设置右侧轴的刻度范围
# ax2.set_ylim([0, max(get_throughput) * 1.1])

# # 设置横坐标标签
# ax1.set_xticks(np.arange(len(cachepolicy)) + bar_width / 2)
# ax1.set_xticklabels(cachepolicy)

# # 添加图例
# ax1.legend(loc='upper left')
# ax2.legend(loc='upper right')

# # 添加标题
# plt.title('Bloomfilter Metrics')

# plt.show()

# # 读取 Compaction 表的数据
# df_compaction = pd.read_excel("./result.xlsx", sheet_name="Compaction")

# # 提取数据
# total_data_size = df_compaction.iloc[:, 0]
# put_latency = df_compaction.iloc[:, 1]

# # 绘制 Compaction 图
# plt.figure(figsize=(10, 6))

# plt.plot(
#     total_data_size, put_latency / 1000, marker="o", color="green", linewidth=3
# )  # 设置折线粗细为3

# plt.xlabel("Total Data Size")
# plt.ylabel("Put Latency (microseconds)")
# plt.title("Compaction Metrics")
# plt.xticks(total_data_size[::4])
# plt.grid(True)
# plt.show()


# 读取 BFsize 表的数据
df_bfsize = pd.read_excel("./result.xlsx", sheet_name="BFsize")

# 提取数据
bfsize = df_bfsize.iloc[:, 0]
put_latency = df_bfsize.iloc[:, 1]
get_latency = df_bfsize.iloc[:, 2]

# 绘制曲线图
plt.figure(figsize=(10, 6))

# 绘制put_latency曲线
plt.plot(bfsize, put_latency, marker="o", color="green", linewidth=3, label="Put Latency")

# 绘制get_latency曲线，使用虚线
plt.plot(bfsize, get_latency, marker="s", linestyle="--", color="blue", linewidth=3, label="Get Latency")

plt.xlabel("BFsize (KB)")
plt.ylabel("Latency (microseconds)")
plt.title("BFsize Metrics")

plt.grid(True)
plt.legend()

plt.show()
