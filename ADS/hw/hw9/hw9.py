import matplotlib.pyplot as plt


def plot_performance(thread_nums, performance_ms):
    # 设置柱状图的宽度
    bar_width = 0.35

    # 设置柱状图的位置
    index = range(len(thread_nums))

    # 创建柱状图
    plt.bar(index, performance_ms, bar_width, label="Performance (ms)")

    # 添加标题和标签
    plt.xlabel("Thread Number")
    plt.ylabel("Performance (ms)")
    plt.title("Performance vs. Thread Number")
    plt.xticks(index, thread_nums)
    plt.legend()

    # 显示柱状图
    plt.tight_layout()
    plt.show()


# 示例数据
thread_nums = [1, 2, 4, 8, 16]
performance_ms = [566.06, 361.72, 229.8, 160.19, 118.42]

# 绘制柱状图
plot_performance(thread_nums, performance_ms)
