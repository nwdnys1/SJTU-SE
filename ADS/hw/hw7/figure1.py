import csv
import matplotlib.pyplot as plt

# 数据
data = {
    "Ordered": {
        "Quick Select": [],
        "Linear Select": []
    },
    "Reverse": {
        "Quick Select": [],
        "Linear Select": []
    },
    "Random": {
        "Quick Select": [],
        "Linear Select": []
    }
}

# 从CSV文件中读取数据并加载到字典中
with open("data1.csv", newline='') as csvfile:
    reader = csv.reader(csvfile)
    next(reader)  # 跳过标题行
    for row in reader:
        data_type, algorithm, data_size, time = row
        data[data_type][algorithm].append((int(data_size), float(time)))

# 绘制图像
for data_type, algorithms in data.items():
    plt.figure()
    plt.title(data_type + " Data")
    for algorithm, values in algorithms.items():
        sizes = [x[0] for x in values]
        times = [x[1] for x in values]
        plt.plot(sizes, times, label=algorithm)
    plt.xlabel("Data Size")
    plt.ylabel("Time (seconds)")
    plt.legend()
    plt.grid(True)
    plt.show()
