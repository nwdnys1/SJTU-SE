from pyspark import SparkContext

# 初始化SparkContext
sc = SparkContext("spark://spark-master:7077", "WordCount")

# 读取文件 rdd形如 ["Java Programming", "Python Programming", "C++ Programming"]
files = ["/mnt/1.txt", "/mnt/2.txt", "/mnt/3.txt"]
rdd = sc.textFile(",".join(files))  # 读取文件并生成 RDD

# 定义关键词列表
keywords = [
    "Java",
    "JavaScript",
    "C++",
    "Python",
    "SQL",
    "HTML",
    "CSS",
    "Android",
    "软件工程",
    "物联网",
    "分布式",
    "编程",
    "计算机",
    "网络",
]


# 对某一行计算关键词出现的次数 返回形如 [("Java", 1), ("Programming", 2)] 的列表
def count_keywords(line, keywords):
    word_cnts = {keyword: line.lower().count(keyword.lower()) for keyword in keywords}
    return [(keyword, count) for keyword, count in word_cnts.items() if count > 0]


# 使用 flatMap 展开每一行的关键词计数 返回形如 [("Java", 1), ("Programming", 2)] 的列表
cnt_map = rdd.flatMap(lambda line: count_keywords(line, keywords))

# 使用 reduceByKey 计算关键词的总次数 返回形如 [("Java", 10), ("Programming", 20)] 的列表
res = cnt_map.reduceByKey(lambda a, b: a + b)

# 输出结果
for keyword, count in res.collect():
    print(f"关键词 '{keyword}' 出现次数: {count}")

sc.stop()
