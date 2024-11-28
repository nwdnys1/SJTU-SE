import pandas as pd
import matplotlib.pyplot as plt

# 读取CSV文件
df = pd.read_csv('data2.csv', header=None, names=['N', 'Q', 'Time'])

# 提取不同数据规模下的数据
data_N1000 = df[df['N'] == 1000]
data_N10000 = df[df['N'] == 10000]
data_N100000 = df[df['N'] == 100000]

# 绘制曲线
plt.figure(figsize=(10, 6))

plt.plot(data_N1000['Q'], data_N1000['Time'], label='N=1000')
plt.plot(data_N10000['Q'], data_N10000['Time'], label='N=10000')
plt.plot(data_N100000['Q'], data_N100000['Time'], label='N=100000')

plt.xlabel('Q')
plt.ylabel('Time')
plt.title('Linear Select Algorithm Time vs. Q for Different N')
plt.legend()
plt.grid(True)

plt.show()
