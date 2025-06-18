import numpy as np

# 定义数据
logits_dict = {
    'dog': 2.1, 'cat': 1.9, 'you': 3.3, 'the': -1.2, 'a': 0.0,
    'hello': -2.9, 'fish': 1.9, 'apple': -0.2, 'banana': 0.8, 'egg': 1.1
}

# 转换为NumPy数组
tokens = list(logits_dict.keys())
logits = np.array(list(logits_dict.values()))

# 标准softmax
def softmax(x):
    e_x = np.exp(x - np.max(x))  # 防止数值溢出
    return e_x / e_x.sum()

# 带温度的softmax
def softmax_with_temp(x, temperature=1.0):
    x = x / temperature  # 应用温度
    e_x = np.exp(x - np.max(x))  # 数值稳定化
    return e_x / e_x.sum()

# (a) 标准softmax计算并排序
probs = softmax(logits)
sorted_idx = np.argsort(probs)[::-1]  # 降序排列索引

print("(a) 标准softmax结果（降序排列）：")
for i in sorted_idx:
    print(f"{tokens[i]}: {probs[i]:.4f}")

# (b) 温度=2.0的softmax计算并排序
temp = 2.0
probs_temp = softmax_with_temp(logits, temperature=temp)
sorted_idx_temp = np.argsort(probs_temp)[::-1]

print("\n(b) 温度=2.0的softmax结果（降序排列）：")
for i in sorted_idx_temp:
    print(f"{tokens[i]}: {probs_temp[i]:.4f}")

# (c) 应用top_k=5
top_k = 5
top_k_indices = sorted_idx_temp[:top_k]  # 取前5个最高概率的token
top_k_probs = probs_temp[top_k_indices]
top_k_probs = top_k_probs / top_k_probs.sum()  # 重新归一化

print("\n(c) 应用top_k=5后的结果：")
for i, idx in enumerate(top_k_indices):
    print(f"{tokens[idx]}: {top_k_probs[i]:.4f}")

# (d) 应用top_p=0.5
top_p = 0.5
sorted_probs = np.sort(probs_temp)[::-1]  # 已排序的概率
cumulative_probs = np.cumsum(sorted_probs)  # 累积概率

# 找到第一个使累积概率≥top_p的位置
n_keep = np.argmax(cumulative_probs >= top_p) + 1
top_p_indices = sorted_idx_temp[:n_keep]  # 保留的token索引
top_p_probs = probs_temp[top_p_indices]
top_p_probs = top_p_probs / top_p_probs.sum()  # 重新归一化

print("\n(d) 应用top_p=0.5后的结果：")
for i, idx in enumerate(top_p_indices):
    print(f"{tokens[idx]}: {top_p_probs[i]:.4f}")
    

b = [9*3**0.5, 18*3**0.5, 27*3**0.5, 36*3**0.5]
print(b)
softmax_b = softmax(np.array(b))
print("\nSoftmax of b:")
for i, val in enumerate(softmax_b):
    print(f"b[{i}]: {val:.4f}")