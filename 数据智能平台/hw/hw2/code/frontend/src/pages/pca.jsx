import React, { useState, useEffect } from "react";
import BasicLayout from "../layout/basicLayout";

export default function PcaPage() {
  const [files, setFiles] = useState([]);
  const [classifiedFiles, setClassifiedFiles] = useState({
    发电机: [],
    冷机: [],
    锅炉: [],
    三联供: [],
  });
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState(null);
  const [selectedFile, setSelectedFile] = useState(null); // 存储当前选中的文件

  useEffect(() => {
    // 调用后端 API 获取文件列表
    fetch("http://10.119.15.62:5000/api/html/all_pca_files")
      .then((response) => {
        if (!response.ok) {
          throw new Error("无法加载文件列表，请检查网络连接或服务器状态。");
        }
        return response.json();
      })
      .then((data) => {
        setFiles(data);
        setLoading(false);
      })
      .catch((error) => {
        console.error("Error fetching files:", error);
        setError(error.message);
        setLoading(false);
      });
  }, []);

  useEffect(() => {
    if (files.length > 0) {
      // 根据文件名分类
      const categories = {
        发电机: [],
        冷机: [],
        锅炉: [],
        三联供: [],
      };

      files.forEach((file) => {
        const fileName = file.name.replace("_report.html", ""); // 去掉 "_report.html" 后缀
        if (fileName.includes("发电机")) {
          categories.发电机.push(file);
        } else if (fileName.includes("冷机")) {
          categories.冷机.push(file);
        } else if (fileName.includes("锅炉")) {
          categories.锅炉.push(file);
        } else if (fileName.includes("三联供")) {
          categories.三联供.push(file);
        }
      });

      // 按照文件名称排序
      Object.keys(categories).forEach((category) => {
        categories[category].sort((a, b) => {
          const nameA = a.name.replace("_report.html", "");
          const nameB = b.name.replace("_report.html", "");
          return nameA.localeCompare(nameB);
        });
      });

      setClassifiedFiles(categories);
    }
  }, [files]);

  if (loading) {
    return (
      <BasicLayout>
        <div
          style={{
            display: "flex",
            flexDirection: "column",
            alignItems: "center",
            justifyContent: "center",
            height: "100vh",
            fontFamily: "Arial, sans-serif",
          }}
        >
          <h1 style={{ fontSize: "24px", marginBottom: "16px" }}>加载中...</h1>
          <div
            style={{
              border: "4px solid #f3f3f3",
              borderTop: "4px solid #3498db",
              borderRadius: "50%",
              width: "40px",
              height: "40px",
              animation: "spin 1s linear infinite",
            }}
          ></div>
          <style jsx>{`
            @keyframes spin {
              0% {
                transform: rotate(0deg);
              }
              100% {
                transform: rotate(360deg);
              }
            }
          `}</style>
        </div>
      </BasicLayout>
    );
  }

  if (error) {
    return (
      <BasicLayout>
        <div
          style={{
            display: "flex",
            flexDirection: "column",
            alignItems: "center",
            justifyContent: "center",
            height: "100vh",
            fontFamily: "Arial, sans-serif",
            color: "#e74c3c",
          }}
        >
          <h1 style={{ fontSize: "24px", marginBottom: "16px" }}>加载失败</h1>
          <p style={{ fontSize: "16px", marginBottom: "24px" }}>{error}</p>
          <button
            onClick={() => window.location.reload()}
            style={{
              padding: "10px 20px",
              fontSize: "16px",
              backgroundColor: "#3498db",
              color: "#fff",
              border: "none",
              borderRadius: "5px",
              cursor: "pointer",
              transition: "background-color 0.3s",
            }}
            onMouseOver={(e) => (e.target.style.backgroundColor = "#2980b9")}
            onMouseOut={(e) => (e.target.style.backgroundColor = "#3498db")}
          >
            刷新页面
          </button>
        </div>
      </BasicLayout>
    );
  }

  return (
    <BasicLayout>
      <div
        style={{
          padding: "20px",
          fontFamily: "Arial, sans-serif",
        }}
      >
        <h1
          style={{
            fontSize: "28px",
            marginBottom: "20px",
            textAlign: "center",
          }}
        >
          PCA分析结果列表
        </h1>
        {/* iframe 展示选中的文件 */}
        {selectedFile && (
          <div
            style={{
              marginTop: "40px",
              textAlign: "center",
            }}
          >
            <h2
              style={{
                fontSize: "24px",
                marginBottom: "20px",
                color: "#3498db",
              }}
            >
              数据分析
            </h2>
            <iframe
              src={`http://10.119.15.62:5000/api/html/files/${selectedFile}`}
              width="100%"
              height="700px"
              style={{
                border: "1px solid #ddd",
                borderRadius: "5px",
              }}
              title={selectedFile}
            ></iframe>
            <button
              onClick={() => setSelectedFile(null)} // 关闭 iframe
              style={{
                marginTop: "20px",
                padding: "10px 20px",
                fontSize: "16px",
                backgroundColor: "#e74c3c",
                color: "#fff",
                border: "none",
                borderRadius: "5px",
                cursor: "pointer",
                transition: "background-color 0.3s",
              }}
              onMouseOver={(e) => (e.target.style.backgroundColor = "#c0392b")}
              onMouseOut={(e) => (e.target.style.backgroundColor = "#e74c3c")}
            >
              关闭预览
            </button>
          </div>
        )}

        {/* 分类显示 */}
        {Object.keys(classifiedFiles).map((category) => (
          <div key={category} style={{ marginBottom: "40px" }}>
            <h2
              style={{
                fontSize: "24px",
                marginBottom: "16px",
                color: "#3498db",
              }}
            >
              {category}
            </h2>
            <ul
              style={{
                listStyleType: "none",
                padding: 0,
                display: "flex",
                flexDirection: "column",
                gap: "10px",
              }}
            >
              {classifiedFiles[category].length > 0 ? (
                classifiedFiles[category].map((file) => (
                  <li
                    key={file.name}
                    style={{
                      display: "flex",
                      alignItems: "center",
                      gap: "10px",
                      padding: "10px",
                      backgroundColor: "#f9f9f9",
                      borderRadius: "5px",
                      boxShadow: "0 2px 4px rgba(0, 0, 0, 0.1)",
                      transition: "background-color 0.3s",
                    }}
                  >
                    <button
                      onClick={() => setSelectedFile(file.name)} // 设置选中的文件
                      style={{
                        padding: "5px 10px",
                        fontSize: "14px",
                        backgroundColor: "#3498db",
                        color: "#fff",
                        border: "none",
                        borderRadius: "3px",
                        cursor: "pointer",
                        transition: "background-color 0.3s",
                      }}
                      onMouseOver={(e) =>
                        (e.target.style.backgroundColor = "#2980b9")
                      }
                      onMouseOut={(e) =>
                        (e.target.style.backgroundColor = "#3498db")
                      }
                    >
                      查看
                    </button>
                    <span style={{ fontSize: "16px", fontWeight: "bold" }}>
                      {file.name}
                    </span>
                  </li>
                ))
              ) : (
                <li style={{ color: "#999", textAlign: "center" }}>暂无文件</li>
              )}
            </ul>
          </div>
        ))}
      </div>
    </BasicLayout>
  );
}
