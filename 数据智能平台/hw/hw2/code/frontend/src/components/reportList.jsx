import React, { useState, useEffect } from "react";
import BasicLayout from "../layout/basicLayout";

export default function ReportList() {
  const [files, setFiles] = useState([]);
  const [classifiedFiles, setClassifiedFiles] = useState({
    三联供: [],
    冷机: [],
    热机: [],
  });
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState(null);

  useEffect(() => {
    // 调用后端 API 获取文件列表
    fetch("http://10.119.15.62:5000/api/html/all_report_files")
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
        三联供: [],
        冷机: [],
        热机: [],
      };

      files.forEach((file) => {
        const fileName = file.name.replace("_report.html", ""); // 去掉 "_report.html" 后缀
        if (fileName.includes("三联供")) {
          categories.三联供.push(file);
        } else if (fileName.includes("冷机")) {
          categories.冷机.push(file);
        } else if (fileName.includes("热机")) {
          categories.热机.push(file); // 将锅炉归类到热机
        }
        // 如果还有其他类别需要处理，可以在这里添加
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
    );
  }

  if (error) {
    return (
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
    );
  }

  return (
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
        报表文件列表
      </h1>

      {/* 分类显示 */}
      {Object.keys(classifiedFiles).map((category) => (
        <div key={category} style={{ marginBottom: "40px" }}>
          <h2
            style={{
              fontSize: "24px",
              marginBottom: "25px",
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
                  onMouseOver={(e) =>
                    (e.target.style.backgroundColor = "#eaf6fc")
                  }
                  onMouseOut={(e) =>
                    (e.target.style.backgroundColor = "#f9f9f9")
                  }
                >
                  <a
                    href={
                      "http://10.119.15.62:5000/api/html/files/" + file.name
                    }
                    target="_blank"
                    rel="noopener noreferrer"
                    style={{
                      textDecoration: "none",
                      color: "#3498db",
                      fontSize: "16px",
                      fontWeight: "bold",
                      transition: "color 0.3s",
                    }}
                    onMouseOver={(e) => (e.target.style.color = "#2980b9")}
                    onMouseOut={(e) => (e.target.style.color = "#3498db")}
                  >
                    {file.name}
                  </a>
                </li>
              ))
            ) : (
              <li style={{ color: "#999", textAlign: "center" }}>暂无文件</li>
            )}
          </ul>
        </div>
      ))}
    </div>
  );
}
