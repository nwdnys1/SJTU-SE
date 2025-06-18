import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from sklearn.preprocessing import StandardScaler
from sklearn.cluster import KMeans
from sklearn.metrics import silhouette_score
from sklearn.decomposition import PCA
import seaborn as sns
import os
from pathlib import Path
from tqdm import tqdm  # 导入tqdm库


# 1. 数据加载与预处理
def load_and_preprocess(
    folder_path, samples_per_file=1000, output_dir="result/k_means"
):
    """从多个CSV文件加载并预处理航班数据"""
    # 创建输出目录
    Path(output_dir).mkdir(parents=True, exist_ok=True)

    # 检查是否已有采样数据
    sampled_file_path = os.path.join(folder_path, "sampled_data.csv")
    if os.path.exists(sampled_file_path):
        print("Loading existing sampled data...")
        df = pd.read_csv(sampled_file_path)
    else:
        print("Creating new sampled data...")
        all_files = [f for f in os.listdir(folder_path) if f.endswith(".csv")]
        sampled_dfs = []

        for file in tqdm(all_files, desc="Sampling files"):  # 添加文件采样进度条
            file_path = os.path.join(folder_path, file)
            df_chunk = pd.read_csv(file_path)

            # 随机采样
            if len(df_chunk) > samples_per_file:
                df_chunk = df_chunk.sample(samples_per_file, random_state=42)
            sampled_dfs.append(df_chunk)

        # 合并所有采样数据
        df = pd.concat(sampled_dfs, ignore_index=True)
        # 保存采样数据以便复用
        df.to_csv(sampled_file_path, index=False)
        print(f"Sampled data saved to {sampled_file_path}")

    # 选择所有数值型特征
    numeric_cols = ["baseFare", "totalFare", "totalTravelDistance", "travelDuration"]
    features = df[numeric_cols]

    # 标准化数据
    scaler = StandardScaler()
    scaled_features = scaler.fit_transform(features)

    return df, scaled_features, numeric_cols


# 2. 寻找最佳聚类数量
def find_optimal_clusters(data, max_k=10, output_dir="result/k_means"):
    """通过肘部法则和轮廓系数确定最佳K值"""
    inertias = []
    silhouette_scores = []
    k_values = range(2, max_k + 1)

    # 添加进度条
    for k in tqdm(k_values, desc="Finding optimal clusters"):
        kmeans = KMeans(n_clusters=k, random_state=42, n_init=10)
        kmeans.fit(data)
        inertias.append(kmeans.inertia_)

        if k > 1:
            silhouette_scores.append(silhouette_score(data, kmeans.labels_))
        else:
            silhouette_scores.append(0)

    # 创建输出目录
    Path(output_dir).mkdir(parents=True, exist_ok=True)

    # 绘制肘部法则图和轮廓系数图
    plt.figure(figsize=(15, 5))

    plt.subplot(1, 2, 1)
    plt.plot(k_values, inertias, "bo-")
    plt.xlabel("Number of clusters (k)")
    plt.ylabel("Inertia")
    plt.title("Elbow Method")

    plt.subplot(1, 2, 2)
    plt.plot(k_values[1:], silhouette_scores[1:], "go-")
    plt.xlabel("Number of clusters (k)")
    plt.ylabel("Silhouette Score")
    plt.title("Silhouette Analysis")

    plt.tight_layout()
    plt.savefig(os.path.join(output_dir, "optimal_clusters.png"))
    plt.close()

    # 自动选择最佳K值
    optimal_k = np.argmax(silhouette_scores) + 2

    return optimal_k, inertias, silhouette_scores


# 3. 聚类分析与可视化
def cluster_analysis(
    data, original_df, feature_names, k=3, output_dir="result/k_means"
):
    """执行聚类并保存可视化结果"""
    # 训练K-Means模型（添加进度条）
    print(f"\nTraining K-Means with k={k}...")
    kmeans = KMeans(n_clusters=k, random_state=42, n_init=10)

    # 使用tqdm包装fit_predict过程
    with tqdm(total=100, desc="Clustering progress") as pbar:
        clusters = kmeans.fit_predict(data)
        pbar.update(100)  # 完成后更新到100%

    original_df["cluster"] = clusters

    # PCA降维可视化
    pca = PCA(n_components=2)
    reduced_data = pca.fit_transform(data)

    # 创建输出目录
    Path(output_dir).mkdir(parents=True, exist_ok=True)

    # 聚类结果散点图
    plt.figure(figsize=(15, 6))

    plt.subplot(1, 2, 1)
    scatter = plt.scatter(
        reduced_data[:, 0], reduced_data[:, 1], c=clusters, cmap="viridis", alpha=0.6
    )
    plt.xlabel("PCA Component 1")
    plt.ylabel("PCA Component 2")
    plt.title(f"K-Means Clustering (k={k})")
    plt.colorbar(scatter, label="Cluster")

    # 聚类特征分布箱线图
    plt.subplot(1, 2, 2)
    cluster_data = original_df[feature_names + ["cluster"]]
    melted_data = pd.melt(
        cluster_data, id_vars="cluster", var_name="feature", value_name="value"
    )

    sns.boxplot(x="cluster", y="value", hue="feature", data=melted_data)
    plt.title("Feature Distribution by Cluster")
    plt.legend(bbox_to_anchor=(1.05, 1), loc="upper left")

    plt.tight_layout()
    plt.savefig(os.path.join(output_dir, f"clustering_k_{k}.png"))
    plt.close()

    # 计算并显示轮廓系数
    silhouette_avg = silhouette_score(data, clusters)
    print(f"Silhouette Score for k={k}: {silhouette_avg:.3f}")

    # 显示聚类中心
    cluster_centers = pd.DataFrame(kmeans.cluster_centers_, columns=feature_names)
    print("\nCluster Centers (Standardized):")
    print(cluster_centers)

    return original_df


# 4. 主函数
def main():
    # 设置文件路径和参数
    folder_path = "flight_prices"  # 包含多个CSV文件的目录
    output_dir = "result/k_means"
    samples_per_file = 1000  # 每个文件采样数量

    # 加载数据
    print("Loading and preprocessing data...")
    df, scaled_data, feature_names = load_and_preprocess(
        folder_path, samples_per_file, output_dir
    )
    print(f"\nSelected features for clustering: {feature_names}")

    # 寻找最佳聚类数量
    print("\nFinding optimal number of clusters...")
    optimal_k, _, _ = find_optimal_clusters(scaled_data, max_k=8, output_dir=output_dir)
    print(f"\nOptimal number of clusters (based on silhouette score): {optimal_k}")

    # 使用最佳K值进行聚类
    print("\nPerforming final clustering with optimal k...")
    clustered_df = cluster_analysis(
        scaled_data, df.copy(), feature_names, k=optimal_k, output_dir=output_dir
    )

    # 测试不同K值的效果（2到8）
    print("\nTesting different k values (2-8) for comparison...")
    for k in tqdm(range(2, 9), desc="Testing k values"):
        _ = cluster_analysis(
            scaled_data, df.copy(), feature_names, k=k, output_dir=output_dir
        )

    # 保存聚类结果
    clustered_df.to_csv(
        os.path.join(output_dir, "flightprices_clustered.csv"), index=False
    )
    print(
        f"\nClustering results saved to {os.path.join(output_dir, 'flightprices_clustered.csv')}"
    )


if __name__ == "__main__":
    main()
