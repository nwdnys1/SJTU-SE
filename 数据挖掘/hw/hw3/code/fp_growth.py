from collections import defaultdict
import matplotlib.pyplot as plt
from pyspark.sql import SparkSession
from pyspark.ml.fpm import FPGrowth
from matplotlib import font_manager
import os

# 确保result目录存在
os.makedirs("result/fp_growth", exist_ok=True)

# 注册字体
try:
    font_manager.fontManager.addfont("/root/fonts/SimHei.ttf")
    plt.rcParams["font.family"] = "SimHei"
    plt.rcParams["axes.unicode_minus"] = False
    print("字体注册成功")
except Exception as e:
    print(f"字体注册失败: {str(e)}")


# 1. 从文件读取数据
def load_transactions(file_path):
    transactions = []
    with open(file_path, "r") as file:
        for line in file:
            # 去除首尾空白，按空格分割，并过滤空字符串
            items = list(filter(None, line.strip().split(" ")))
            if items:  # 忽略空行
                transactions.append(items)
    return transactions


# 2. 修正后的Python原生FP-Growth实现
class FPTreeNode:
    def __init__(self, item, count, parent):
        self.item = item
        self.count = count
        self.parent = parent
        self.children = {}
        self.link = None

    def increment(self):
        self.count += 1


def construct_fp_tree(transactions, min_support):
    item_counts = defaultdict(int)
    for transaction in transactions:
        for item in transaction:
            item_counts[item] += 1

    frequent_items = {item: count for item, count in item_counts.items() 
                      if count >= min_support}
    if not frequent_items:
        return None, None, None

    sorted_items = sorted(frequent_items.keys(), 
                         key=lambda x: (-frequent_items[x], x))

    root = FPTreeNode(None, 0, None)
    header_table = defaultdict(list)
    
    for transaction in transactions:
        filtered = [item for item in transaction if item in frequent_items]
        filtered.sort(key=lambda x: (-frequent_items[x], x))
        
        current_node = root
        for item in filtered:
            if item in current_node.children:
                current_node.children[item].increment()
            else:
                new_node = FPTreeNode(item, 1, current_node)
                current_node.children[item] = new_node
                
                if item in header_table:
                    last_node = header_table[item][-1]
                    last_node.link = new_node
                header_table[item].append(new_node)
            
            current_node = current_node.children[item]
    
    return root, header_table, frequent_items

def mine_fp_tree(header_table, min_support, prefix, frequent_itemsets, frequent_items):
    sorted_items = sorted(header_table.keys(), 
                         key=lambda x: frequent_items[x])
    
    for item in sorted_items:
        new_prefix = prefix.copy()
        new_prefix.add(item)
        support = min(node.count for node in header_table[item])  # 关键修改：取最小计数
        frequent_itemsets.append((new_prefix, support))
        
        conditional_patterns = []
        node = header_table[item][0]
        while node is not None:
            path = []
            current = node.parent
            while current.item is not None:
                path.append(current.item)
                current = current.parent
            if path:
                conditional_patterns.append((path, node.count))
            node = node.link
        
        conditional_transactions = []
        for pattern, count in conditional_patterns:
            conditional_transactions.extend([pattern] * count)
            
        if conditional_transactions:
            conditional_tree, conditional_header, _ = construct_fp_tree(
                conditional_transactions, min_support)
            
            if conditional_header is not None:
                mine_fp_tree(conditional_header, min_support, new_prefix, 
                           frequent_itemsets, frequent_items)

def fp_growth(transactions, min_support):
    item_counts = defaultdict(int)
    for transaction in transactions:
        for item in transaction:
            item_counts[item] += 1
    
    frequent_items = {item: count for item, count in item_counts.items() 
                      if count >= min_support}
    if not frequent_items:
        return []
    
    fp_tree, header_table, _ = construct_fp_tree(transactions, min_support)
    
    frequent_itemsets = []
    mine_fp_tree(header_table, min_support, set(), frequent_itemsets, frequent_items)
    
    # 添加单项集（使用原始计数）
    for item, count in frequent_items.items():
        frequent_itemsets.append(({item}, count))
    
    # 去重并按支持度排序
    unique_itemsets = []
    seen = set()
    for itemset, support in frequent_itemsets:
        frozen = frozenset(itemset)
        if frozen not in seen:
            seen.add(frozen)
            unique_itemsets.append((itemset, support))
    
    # 修正支持度计数
    final_itemsets = []
    for itemset, _ in unique_itemsets:
        min_support = float('inf')
        for transaction in transactions:
            if itemset.issubset(set(transaction)):
                min_support = min(min_support, sum(1 for t in transactions if itemset.issubset(set(t))))
        final_itemsets.append((itemset, min_support))
    
    return final_itemsets

def main():
    # 从文件加载数据
    file_path = "data/sample_fpgrowth.txt"
    transactions = load_transactions(file_path)

    if not transactions:
        print("错误：文件为空或格式不正确")
        return

    print("从文件加载的事务数据:")
    for i, trans in enumerate(transactions, 1):
        print(f"事务{i}: {trans}")

    min_support = 3  # 最小支持度计数

    # Python原生FP-Growth
    print("\nPython原生FP-Growth算法结果:")
    py_frequent_itemsets = fp_growth(transactions, min_support)
    for itemset, support in sorted(
        py_frequent_itemsets, key=lambda x: (-x[1], len(x[0]), sorted(x[0]))
    ):
        print(f"{sorted(itemset)}: {support}")

    # Spark MLlib FP-Growth
    spark = (
        SparkSession.builder.appName("FPGrowthExample").master("local[*]").getOrCreate()
    )

    # 直接设置日志级别
    spark.sparkContext.setLogLevel("WARN")  # 只显示WARN及以上级别
    try:
        # 准备Spark DataFrame
        df = spark.createDataFrame([(trans,) for trans in transactions], ["items"])

        # 计算最小支持度比例
        min_support_ratio = min_support / len(transactions)

        spark_fp_growth = FPGrowth(
            itemsCol="items", minSupport=min_support_ratio, minConfidence=0.6
        )
        model = spark_fp_growth.fit(df)

        spark_frequent_itemsets = model.freqItemsets.collect()

        print("\nSpark MLlib FP-Growth算法结果:")
        for itemset in sorted(
            spark_frequent_itemsets,
            key=lambda x: (-x["freq"], len(x["items"]), sorted(x["items"])),
        ):
            print(f"{sorted(itemset['items'])}: {itemset['freq']}")

        # 结果比较
        print("\n结果比较:")
        print("Python原生实现找到的频繁项集数量:", len(py_frequent_itemsets))
        print("Spark MLlib实现找到的频繁项集数量:", len(spark_frequent_itemsets))

        # 可视化
        def prepare_data(itemsets):
            data = {}
            if isinstance(itemsets[0], tuple):  # Python原生格式
                for itemset, support in itemsets:
                    key = ", ".join(sorted(itemset))
                    data[key] = support
            else:  # Spark格式
                for itemset in itemsets:
                    key = ", ".join(sorted(itemset["items"]))
                    data[key] = itemset["freq"]
            return data

        py_data = prepare_data(py_frequent_itemsets)
        spark_data = prepare_data(spark_frequent_itemsets)

        all_itemsets = set(py_data.keys()).union(set(spark_data.keys()))
        labels = sorted(
            all_itemsets,
            key=lambda x: (-max(py_data.get(x, 0), spark_data.get(x, 0)), x),
        )
        py_values = [py_data.get(x, 0) for x in labels]
        spark_values = [spark_data.get(x, 0) for x in labels]

        x = range(len(labels))
        width = 0.35

        plt.figure(figsize=(12, 6))
        plt.bar([i - width / 2 for i in x], py_values, width, label="Python原生")
        plt.bar([i + width / 2 for i in x], spark_values, width, label="Spark MLlib")

        plt.xlabel("频繁项集")
        plt.ylabel("支持度计数")
        plt.title("FP-Growth算法结果比较")
        plt.xticks(x, labels, rotation=45, ha="right")
        plt.legend()
        plt.tight_layout()

        # 保存图像而不是显示
        plt.savefig("result/fp_growth/fp_growth_comparison.png")
        print("\n可视化结果已保存到 result/fp_growth_comparison.png")

        # 保存结果到文件
        with open("result/fp_growth/python_results.txt", "w") as f:
            f.write("Python原生FP-Growth算法结果:\n")
            for itemset, support in sorted(
                py_frequent_itemsets, key=lambda x: (-x[1], len(x[0]), sorted(x[0]))
            ):
                f.write(f"{sorted(itemset)}: {support}\n")

        with open("result/fp_growth/spark_results.txt", "w") as f:
            f.write("Spark MLlib FP-Growth算法结果:\n")
            for itemset in sorted(
                spark_frequent_itemsets,
                key=lambda x: (-x["freq"], len(x["items"]), sorted(x["items"])),
            ):
                f.write(f"{sorted(itemset['items'])}: {itemset['freq']}\n")

        print("详细结果已保存到 result")

    finally:
        spark.stop()


if __name__ == "__main__":
    main()
