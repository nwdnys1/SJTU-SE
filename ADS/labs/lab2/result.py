import matplotlib.pyplot as plt

# 准备数据点
thread_nums = [1, 2, 3, 4, 5, 6, 7, 8, 16, 32, 48, 64, 80, 100]
query_times = [848.1, 426.3, 287.9, 226.4, 207.1, 182.7, 168.7, 153.8, 151.8, 148.4, 163.1, 182.162, 163.6, 260.4]

# 绘制线图
plt.plot(thread_nums, query_times, marker='o')

# 添加标题与标签
plt.title('Query Time vs. Thread Numbers')
plt.xlabel('Thread Numbers')
plt.ylabel('Query Time (μs)')

# 显示图例
plt.legend(['Query Time'])

# 显示图表
plt.show()

# 准备数据点
M = [10, 20, 30, 40, 50]
recall = [0.971, 1, 1, 1, 1]
query_time = [0.5, 0.7, 0.9, 1.1, 1.3]

# 创建画布和子图
fig, ax1 = plt.subplots()

# 绘制召回率曲线
ax1.plot(M, recall, marker='o', color='b')
ax1.set_xlabel('M')
ax1.set_ylabel('Recall', color='b')

# 创建第二个坐标轴
ax2 = ax1.twinx()

# 绘制查询耗时曲线
ax2.plot(M, query_time, marker='o', color='r')
ax2.set_ylabel('Query Time (ms)', color='r')

# 设置图例
line1, = ax1.plot([], [], marker='o', color='b', label='Recall')
line2, = ax2.plot([], [], marker='o', color='r', label='Query Time (ms)')
lines = [line1, line2]
labels = [line.get_label() for line in lines]
ax1.legend(lines, labels, loc='best')

# 添加标题
plt.title('Recall and Query Time vs. M')

# 显示图表
plt.show()