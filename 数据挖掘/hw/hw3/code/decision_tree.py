import os
import numpy as np
import pandas as pd
from collections import defaultdict
from sklearn.model_selection import train_test_split
from sklearn.metrics import accuracy_score, confusion_matrix
import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec
import seaborn as sns
from pyspark.sql import SparkSession
from pyspark.ml.classification import DecisionTreeClassifier
from pyspark.ml.feature import VectorAssembler
from tqdm import tqdm  # 新增导入


class DataLoader:
    """数据加载与预处理类"""

    @staticmethod
    def load_sparse_data(file_path):
        """加载稀疏格式的分类数据"""
        data = []
        labels = []
        max_feature = 0

        # 添加进度条
        with open(file_path, "r") as f:
            lines = f.readlines()
            for line in tqdm(lines, desc="Loading sparse data"):
                parts = line.strip().split()
                labels.append(int(parts[0]))
                features = defaultdict(float)

                for kv in parts[1:]:
                    k, v = kv.split(":")
                    k = int(k)
                    v = float(v)
                    features[k] = v
                    if k > max_feature:
                        max_feature = k

                data.append(features)

        # 转换为密集矩阵
        dense_data = np.zeros((len(data), max_feature + 1))
        for i, features in tqdm(enumerate(data), desc="Converting to dense matrix"):
            for k, v in features.items():
                dense_data[i, k] = v

        return np.array(labels), dense_data

    @staticmethod
    def load_flight_data(folder_path, samples_per_file=10000, force_resample=False):
        """加载航班数据，带有采样持久化功能"""
        # 定义采样后的文件保存路径
        sampled_file = os.path.join(folder_path, "SAMPLED_DATA.csv")
        
        # 如果采样文件已存在且不需要重新采样，直接加载
        if os.path.exists(sampled_file) and not force_resample:
            print(f"Loading pre-sampled data from {sampled_file}")
            df = pd.read_csv(sampled_file)
        else:
            # 否则执行采样流程
            print(f"Sampling data (samples_per_file={samples_per_file})...")
            all_files = [f for f in os.listdir(folder_path) 
                        if f.endswith(".csv") and not f.startswith("_")]
            sampled_dfs = []
            
            for file in tqdm(all_files, desc="Sampling CSV files"):
                file_path = os.path.join(folder_path, file)
                df_chunk = pd.read_csv(file_path)
                
                if len(df_chunk) > samples_per_file:
                    df_chunk = df_chunk.sample(samples_per_file, random_state=42)
                sampled_dfs.append(df_chunk)
            
            # 合并并保存采样数据
            df = pd.concat(sampled_dfs, ignore_index=True)
            df.to_csv(sampled_file, index=False)
            print(f"Saved sampled data to {sampled_file}")
        
        # 提取特征和标签
        X = df[["baseFare", "totalTravelDistance", "travelDuration"]]
        y = df["isNonStop"].astype(int)
        
        return X, y


class DecisionTree:
    """Python原生决策树实现"""

    def __init__(self, max_depth=8, min_samples_split=2):
        self.max_depth = max_depth
        self.min_samples_split = min_samples_split
        self.tree = None

    def _gini(self, y):
        """计算Gini系数"""
        classes = np.unique(y)
        gini = 1.0
        for c in classes:
            p = np.sum(y == c) / len(y)
            gini -= p**2
        return gini

    def _best_split(self, X, y):
        """寻找最佳分割点"""
        best_gini = float("inf")
        best_feature, best_value = None, None

        # 添加进度条
        for feature in tqdm(range(X.shape[1]), desc="Finding best split", leave=False):
            unique_values = np.unique(X[:, feature])
            for value in unique_values:
                left_indices = X[:, feature] <= value
                right_indices = ~left_indices

                if np.sum(left_indices) == 0 or np.sum(right_indices) == 0:
                    continue

                gini_left = self._gini(y[left_indices])
                gini_right = self._gini(y[right_indices])

                weighted_gini = (
                    np.sum(left_indices) * gini_left
                    + np.sum(right_indices) * gini_right
                ) / len(y)

                if weighted_gini < best_gini:
                    best_gini = weighted_gini
                    best_feature = feature
                    best_value = value

        return best_feature, best_value

    def _build_tree(self, X, y, depth=0):
        """递归构建决策树"""
        if (
            depth >= self.max_depth
            or len(y) < self.min_samples_split
            or len(np.unique(y)) == 1
        ):
            return np.argmax(np.bincount(y))

        feature, value = self._best_split(X, y)
        if feature is None:
            return np.argmax(np.bincount(y))

        left_indices = X[:, feature] <= value
        right_indices = ~left_indices

        left_subtree = self._build_tree(X[left_indices], y[left_indices], depth + 1)
        right_subtree = self._build_tree(X[right_indices], y[right_indices], depth + 1)

        return {
            "feature": feature,
            "value": value,
            "left": left_subtree,
            "right": right_subtree,
        }

    def fit(self, X, y):
        """训练模型"""
        print("Building decision tree...")
        self.tree = self._build_tree(X, y)

    def _predict_sample(self, sample, tree):
        """预测单个样本"""
        if isinstance(tree, dict):
            if sample[tree["feature"]] <= tree["value"]:
                return self._predict_sample(sample, tree["left"])
            else:
                return self._predict_sample(sample, tree["right"])
        else:
            return tree

    def predict(self, X):
        """批量预测"""
        return np.array(
            [self._predict_sample(x, self.tree) for x in tqdm(X, desc="Predicting")]
        )


class SparkDecisionTree:
    """Spark MLlib决策树封装类"""
    
    def __init__(self, driver_memory="4g", executor_memory="4g"):
        self.spark = (
            SparkSession.builder.appName("DecisionTreeComparison")
            .master("local[*]")
            .config("spark.driver.memory", driver_memory)
            .config("spark.executor.memory", executor_memory)
            .config("spark.memory.offHeap.enabled", "true")
            .config("spark.memory.offHeap.size", "2g")
            .config("spark.sql.shuffle.partitions", "200")
            .config("spark.hadoop.fs.defaultFS", "file:///")
            .getOrCreate()
        )
        self.spark.sparkContext.setLogLevel("WARN")
        self.model = None  # 新增属性，用于保存模型
    
    def train_predict(self, X, y, feature_cols, label_col="label"):
        """训练并预测"""
        # 转换为Spark DataFrame
        print("Converting data to Spark DataFrame...")
        data = pd.DataFrame(X, columns=feature_cols)
        data[label_col] = y
        df = self.spark.createDataFrame(data)

        # 特征向量化
        print("Vectorizing features...")
        assembler = VectorAssembler(inputCols=feature_cols, outputCol="features")
        df = assembler.transform(df)

        # 划分训练测试集
        print("Splitting data...")
        train, test = df.randomSplit([0.7, 0.3], seed=42)

        # 训练模型
        print("Training Spark Decision Tree...")
        dt = DecisionTreeClassifier(labelCol=label_col, featuresCol="features")
        self.model = dt.fit(train)  # 保存模型

        # 预测并收集结果
        print("Making predictions...")
        predictions = self.model.transform(test)

        y_pred = []
        y_true = []
        for row in tqdm(
            predictions.select("prediction", label_col).collect(),
            desc="Collecting predictions",
        ):
            y_pred.append(row["prediction"])
            y_true.append(row[label_col])

        return np.array(y_true), np.array(y_pred)
    
    def get_tree_structure(self):
        """获取Spark决策树的结构"""
        if self.model:
            return self.model.toDebugString
        return None
    
    def close(self):
        """关闭Spark会话"""
        self.spark.stop()


class Evaluator:
    """评估与可视化类"""
    
    @staticmethod
    def ensure_dir(directory):
        """确保目录存在"""
        if not os.path.exists(directory):
            os.makedirs(directory)
    
    @staticmethod
    def plot_confusion_matrices(y_true_py, y_pred_py, y_true_spark, y_pred_spark, dataset_name):
        """绘制两种算法的混淆矩阵对比图"""
        fig = plt.figure(figsize=(16, 6))
        gs = gridspec.GridSpec(1, 2, width_ratios=[1, 1])
        
        # Python原生决策树的混淆矩阵
        ax1 = plt.subplot(gs[0])
        cm_py = confusion_matrix(y_true_py, y_pred_py)
        acc_py = accuracy_score(y_true_py, y_pred_py)
        sns.heatmap(cm_py, annot=True, fmt="d", cmap="Blues", ax=ax1)
        ax1.set_title(f"Python Decision Tree\nAccuracy: {acc_py:.2f}")
        ax1.set_xlabel("Predicted")
        ax1.set_ylabel("True")
        
        # Spark决策树的混淆矩阵
        ax2 = plt.subplot(gs[1])
        cm_spark = confusion_matrix(y_true_spark, y_pred_spark)
        acc_spark = accuracy_score(y_true_spark, y_pred_spark)
        sns.heatmap(cm_spark, annot=True, fmt="d", cmap="Greens", ax=ax2)
        ax2.set_title(f"Spark Decision Tree\nAccuracy: {acc_spark:.2f}")
        ax2.set_xlabel("Predicted")
        ax2.set_ylabel("True")
        
        plt.suptitle(f"Confusion Matrices Comparison - {dataset_name}", fontsize=14)
        plt.tight_layout()
        
        # 保存图像
        output_dir = "result/decision_tree"
        Evaluator.ensure_dir(output_dir)
        filename = f"{dataset_name.replace(' ', '_')}_confusion_matrices.png"
        filepath = os.path.join(output_dir, filename)
        plt.savefig(filepath)
        plt.close()
        
        print(f"Saved confusion matrices comparison to: {filepath}")
        return acc_py, acc_spark
    
    @staticmethod
    def plot_decision_trees(python_tree, spark_tree_structure, feature_names, dataset_name, max_depth=10):
        """绘制两种算法的决策树对比图"""
        fig = plt.figure(figsize=(20, 10))
        gs = gridspec.GridSpec(1, 2, width_ratios=[1, 1])
        
        # Python原生决策树可视化（自定义绘制）
        ax1 = plt.subplot(gs[0])
        Evaluator._plot_custom_tree(ax1, python_tree, feature_names, max_depth)
        ax1.set_title("Python Decision Tree Structure")
        
        # Spark决策树可视化（文本形式）
        ax2 = plt.subplot(gs[1])
        ax2.text(0.5, 0.5, spark_tree_structure, 
                fontfamily='monospace', 
                fontsize=10,
                verticalalignment='center',
                horizontalalignment='center',
                bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.5))
        ax2.axis('off')
        ax2.set_title("Spark Decision Tree Structure")
        
        plt.suptitle(f"Decision Trees Comparison - {dataset_name}", fontsize=14)
        plt.tight_layout()
        
        # 保存图像
        output_dir = "result/decision_tree"
        Evaluator.ensure_dir(output_dir)
        filename = f"{dataset_name.replace(' ', '_')}_decision_trees.png"
        filepath = os.path.join(output_dir, filename)
        plt.savefig(filepath)
        plt.close()
        
        print(f"Saved decision trees comparison to: {filepath}")
    
    @staticmethod
    def _plot_custom_tree(ax, tree, feature_names, max_depth, depth=0, pos=(0.5, 1.0), width=0.5):
        """递归绘制自定义决策树"""
        if depth > max_depth:
            return
        
        if isinstance(tree, dict):
            # 绘制决策节点
            node_text = f"X[{tree['feature']}] <= {tree['value']:.2f}\n{feature_names[tree['feature']]}"
            ax.text(pos[0], pos[1], node_text, 
                   ha='center', va='center',
                   bbox=dict(boxstyle='round', facecolor='white', alpha=0.8))
            
            # 计算子节点位置
            left_pos = (pos[0] - width/2, pos[1] - 0.15)
            right_pos = (pos[0] + width/2, pos[1] - 0.15)
            
            # 绘制连接线
            ax.plot([pos[0], left_pos[0]], [pos[1], left_pos[1]], 'k-')
            ax.plot([pos[0], right_pos[0]], [pos[1], right_pos[1]], 'k-')
            
            # 递归绘制子树
            Evaluator._plot_custom_tree(ax, tree['left'], feature_names, max_depth, depth+1, left_pos, width/2)
            Evaluator._plot_custom_tree(ax, tree['right'], feature_names, max_depth, depth+1, right_pos, width/2)
        else:
            # 绘制叶节点
            ax.text(pos[0], pos[1], f"Class: {tree}", 
                   ha='center', va='center',
                   bbox=dict(boxstyle='round', facecolor='lightgreen', alpha=0.8))
        
        ax.axis('off')
    
    @staticmethod
    def compare_results(results):
        """对比不同算法的结果并保存可视化"""
        plt.figure(figsize=(10, 6))
        datasets = list(results.keys())
        python_acc = [results[d]["python"] for d in datasets]
        spark_acc = [results[d]["spark"] for d in datasets]

        x = range(len(datasets))
        plt.bar([i - 0.2 for i in x], python_acc, width=0.4, label="Python Native")
        plt.bar([i + 0.2 for i in x], spark_acc, width=0.4, label="Spark MLlib")

        plt.xticks(x, datasets)
        plt.ylabel("Accuracy")
        plt.title("Decision Tree Performance Comparison")
        plt.legend()
        plt.ylim(0, 1.1)

        # 保存图像
        output_dir = "result/decision_tree"
        Evaluator.ensure_dir(output_dir)
        filepath = os.path.join(output_dir, "performance_comparison.png")
        plt.savefig(filepath)
        plt.close()

        print(f"Saved comparison visualization to: {filepath}")

def analyze_sparse_data(data_path):
    """封装稀疏数据的分析流程"""
    print("\n正在处理稀疏分类数据...")
    labels, features = DataLoader.load_sparse_data(data_path)
    feature_cols = [f"f_{i}" for i in range(features.shape[1])]

    # Python原生实现
    X_train, X_test, y_train, y_test = train_test_split(
        features, labels, test_size=0.3, random_state=42
    )
    dt = DecisionTree(max_depth=5)
    dt.fit(X_train, y_train)
    y_pred_py = dt.predict(X_test)
    
    # Spark实现
    spark_dt = SparkDecisionTree()
    y_true_spark, y_pred_spark = spark_dt.train_predict(features, labels, feature_cols)
    
    # 评估和可视化
    acc_py, acc_spark = Evaluator.plot_confusion_matrices(
        y_test, y_pred_py, y_true_spark, y_pred_spark, "Sparse Classification Data"
    )
    
    # 决策树可视化
    Evaluator.plot_decision_trees(
        dt.tree, 
        spark_dt.get_tree_structure(), 
        feature_cols, 
        "Sparse Classification Data"
    )
    
    spark_dt.close()
    return {"python": acc_py, "spark": acc_spark}


def analyze_flight_data(folder_path):
    """封装航班数据的分析流程"""
    print("\n正在处理航班数据...")
    X_flight, y_flight = DataLoader.load_flight_data(folder_path)
    feature_cols = ["baseFare", "totalTravelDistance", "travelDuration"]

    # Python原生实现
    X_train, X_test, y_train, y_test = train_test_split(
        X_flight.values, y_flight.values, test_size=0.3, random_state=42
    )
    dt_flight = DecisionTree(max_depth=5)
    dt_flight.fit(X_train, y_train)
    y_pred_py = dt_flight.predict(X_test)
    
    # Spark实现
    spark_dt = SparkDecisionTree()
    y_true_spark, y_pred_spark = spark_dt.train_predict(
        X_flight.values, y_flight.values, feature_cols, "isNonStop"
    )
    
    # 评估和可视化
    acc_py, acc_spark = Evaluator.plot_confusion_matrices(
        y_test, y_pred_py, y_true_spark, y_pred_spark, "Flight Data"
    )
    
    # 决策树可视化
    Evaluator.plot_decision_trees(
        dt_flight.tree, 
        spark_dt.get_tree_structure(), 
        feature_cols, 
        "Flight Data"
    )
    
    spark_dt.close()
    return {"python": acc_py, "spark": acc_spark}


def main():
    # 设置文件路径
    current_dir = os.path.dirname(os.path.abspath(__file__))
    sparse_data_path = os.path.join(current_dir, "data", "sample_classification.txt")
    flight_data_folder = os.path.join(current_dir, "flight_prices")

    # 分析数据并存储结果
    results = {
        "Sparse Data": analyze_sparse_data(sparse_data_path),
        # "Flight Data": analyze_flight_data(flight_data_folder),
    }

    # 结果对比
    print("\n算法性能对比:")
    Evaluator.compare_results(results)


if __name__ == "__main__":
    main()