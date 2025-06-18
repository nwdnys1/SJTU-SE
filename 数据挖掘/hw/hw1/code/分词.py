import pandas as pd
import jieba

# 读取 CSV 文件，假设 'stock_comments_raw.csv' 文件中有一列 'comment' 存放股评数据
stock_comments = pd.read_csv("data/stock_comments_raw.csv")

# 进行中文分词的函数，添加类型检查
def jieba_cut(text):
    # 如果 text 不是字符串类型，则返回空字符串
    if not isinstance(text, str):
        text = str(text)  # 或者直接返回空字符串 text = ''
    # 使用 jieba.cut 进行分词
    return " ".join(jieba.cut(text))

# 假设股评数据存储在 'comment' 列中
stock_comments['segmented_comments'] = stock_comments['title'].apply(jieba_cut)

# 如果需要将结果保存到新的 CSV 文件中
stock_comments.to_csv("output/segmented_stock_comments.csv", index=False)
