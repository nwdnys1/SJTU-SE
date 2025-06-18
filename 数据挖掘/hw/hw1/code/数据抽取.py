import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.ticker import MaxNLocator

# 读取CSV文件
stock_data = pd.read_csv("data/stock_sh000001.csv")
weibo_data = pd.read_csv("data/weibo_user.csv")

# 统计指标分析（例如：数量、均值、中位数、标准差等） 并保存到文件
stock_data_stats = stock_data.describe()
weibo_data_stats = weibo_data.describe()
stock_data_stats.to_csv("output/stock_data_stats.csv")
weibo_data_stats.to_csv("output/weibo_data_stats.csv")

# 将 'date' 列转换为 datetime 格式，以便更好地处理
stock_data["date"] = pd.to_datetime(stock_data["date"])

# 每个窗口显示3张图，总共3个窗口
# 第一个窗口
fig1, axs1 = plt.subplots(1, 3, figsize=(18, 6))  # 1行3列的子图
axs1 = axs1.ravel()  # 将二维数组展平成一维，方便访问每个子图

# 绘制 'open' 列
axs1[0].plot(stock_data["date"], stock_data["open"], label="Open Price", color="green")
axs1[0].set_title("Stock SH000001 Open Price")
axs1[0].set_xlabel("Date")
axs1[0].set_ylabel("Open Price")
axs1[0].tick_params(axis='x', rotation=45)
axs1[0].grid(True)

# 绘制 'high' 列
axs1[1].plot(stock_data["date"], stock_data["high"], label="High Price", color="red")
axs1[1].set_title("Stock SH000001 High Price")
axs1[1].set_xlabel("Date")
axs1[1].set_ylabel("High Price")
axs1[1].tick_params(axis='x', rotation=45)
axs1[1].grid(True)

# 绘制 'low' 列
axs1[2].plot(stock_data["date"], stock_data["low"], label="Low Price", color="orange")
axs1[2].set_title("Stock SH000001 Low Price")
axs1[2].set_xlabel("Date")
axs1[2].set_ylabel("Low Price")
axs1[2].tick_params(axis='x', rotation=45)
axs1[2].grid(True)

# 第二个窗口
fig2, axs2 = plt.subplots(1, 3, figsize=(18, 6))  # 1行3列的子图
axs2 = axs2.ravel()  # 将二维数组展平成一维，方便访问每个子图

# 绘制 'close' 列
axs2[0].plot(stock_data["date"], stock_data["close"], label="Close Price", color="blue")
axs2[0].set_title("Stock SH000001 Close Price")
axs2[0].set_xlabel("Date")
axs2[0].set_ylabel("Close Price")
axs2[0].tick_params(axis='x', rotation=45)
axs2[0].grid(True)

# 绘制 'volume' 列 ji
axs2[1].plot(stock_data["date"], stock_data["volume"], label="Volume", color="purple")
axs2[1].set_title("Stock SH000001 Volume")
axs2[1].set_xlabel("Date")
axs2[1].set_ylabel("Volume")
axs2[1].tick_params(axis='x', rotation=45)
axs2[1].grid(True)

# 绘制微博数据相关图
axs2[2].bar(weibo_data.index, weibo_data["post_num"], color="skyblue", alpha=0.7)
axs2[2].set_title("Post Number Distribution")
axs2[2].set_xlabel("User Index")
axs2[2].set_ylabel("Number of Posts")
axs2[2].xaxis.set_major_locator(MaxNLocator(integer=True, prune='both', nbins=6))

# 第三个窗口
fig3, axs3 = plt.subplots(1, 3, figsize=(18, 6))  # 1行3列的子图
axs3 = axs3.ravel()  # 将二维数组展平成一维，方便访问每个子图

# 1. 用户性别分布
gender_count = weibo_data["gender"].value_counts()
axs3[0].bar(gender_count.index, gender_count.values, color=["blue", "pink"], alpha=0.7)
axs3[0].set_title("Gender Distribution")
axs3[0].set_xlabel("Gender")
axs3[0].set_ylabel("Count")

# 2. 粉丝数量和帖子数量的关系（散点图）
axs3[1].scatter(
    weibo_data["post_num"], weibo_data["follower_num"], alpha=0.6, color="green"
)
axs3[1].set_title("Posts vs Followers")
axs3[1].set_xlabel("Number of Posts")
axs3[1].set_ylabel("Number of Followers")

# 3. 粉丝数量的分布（柱状图）
axs3[2].bar(weibo_data.index, weibo_data["follower_num"], color="orange", alpha=0.7)
axs3[2].set_title("Followers Number Distribution")
axs3[2].set_xlabel("User Index")
axs3[2].set_ylabel("Number of Followers")
axs3[2].xaxis.set_major_locator(MaxNLocator(integer=True, prune='both', nbins=6))

# 调整布局
plt.tight_layout()  # 自动调整子图间的间距

# 展示所有图形
plt.show()
