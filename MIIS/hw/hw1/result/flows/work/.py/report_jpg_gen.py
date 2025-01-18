import json
import matplotlib.pyplot as plt
from matplotlib import rcParams
from matplotlib.ticker import MaxNLocator
import numpy as np

# 设置字体为黑体
rcParams['font.sans-serif'] = ['SimHei']  # 使用黑体
rcParams['axes.unicode_minus'] = False  # 解决负号显示问题

def plot_buyers_pie_chart(file_path, save_path):
    # 读取买方数据
    with open(file_path, 'r', encoding='utf-8') as file:
        buyers_data = json.load(file)

    # 提取买方名称和总开销
    buyers = [buyer['name'] for buyer in buyers_data]
    total_amounts = [buyer['amount'] for buyer in buyers_data]

    # 绘制饼图
    plt.figure(figsize=(8, 8))
    plt.pie(total_amounts, labels=buyers, autopct='%1.1f%%', startangle=140)
    plt.title('买方开销分布', fontsize=20)
    plt.axis('equal')  # 使饼图为正圆形

    # 保存饼图
    plt.savefig(save_path,dpi = 300)
    plt.close()  # 关闭当前图形

def plot_sellers_pie_chart(file_path, save_path):
    # 读取卖方数据
    with open(file_path, 'r', encoding='utf-8') as file:
        sellers_data = json.load(file)

    # 提取卖方名称和总开销
    sellers = [seller['name'] for seller in sellers_data]
    total_amounts = [seller['amount'] for seller in sellers_data]

    # 绘制饼图
    plt.figure(figsize=(8, 8))
    plt.pie(total_amounts, labels=sellers, autopct='%1.1f%%', startangle=140)
    plt.title('卖方收入分布', fontsize=20)
    plt.axis('equal')  # 使饼图为正圆形

    # 保存饼图
    plt.savefig(save_path,dpi = 300)
    plt.close()  # 关闭当前图形

def plot_invoice_distribution(file_path, save_path):
    # 从文件中读取 JSON 数据
    with open(file_path, 'r', encoding='utf-8') as f:
        invoices = json.load(f)

    # 提取发票金额，注意要进行过滤、如果approval_status为Manual Review则不计入
    amounts = [invoice['amount'] for invoice in invoices if invoice['status'] != '人工审批']

    # 设置每500为一个区间
    bins = np.arange(0, max(amounts) + 500, 500)

    # 绘制柱状图
    plt.hist(amounts, bins=bins, edgecolor='black', histtype='bar')

    # 设置标题和标签
    plt.title('发票金额分布', fontsize=20)
    plt.xlabel('金额区间 (元)', fontsize=14)
    plt.ylabel('发票数量', fontsize=14)

    # 设置 y 轴的刻度为整数
    plt.gca().yaxis.set_major_locator(MaxNLocator(integer=True))

    # 设置 x 轴的刻度为500的倍数
    plt.xticks(np.arange(0, max(amounts) + 500, 500))

    # 显示网格
    plt.grid(axis='y', alpha=0.75)

    # 保存图形，设置 dpi
    plt.savefig(save_path, dpi=300)  # 保存时设置分辨率

# 调用函数，指定文件路径和保存路径
plot_buyers_pie_chart('./.json/buyer.json', './.jpg/买方开销分布.jpg')
plot_sellers_pie_chart('./.json/seller.json', './.jpg/卖方收入分布.jpg')
plot_invoice_distribution('./.json/invoice.json', './.jpg/发票金额分布.jpg')
