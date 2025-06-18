import React from "react";
import { Gauge } from "@ant-design/plots";

const CustomGauge = ({
  width,
  height,
  autoFit,
  target,
  total,
  name,
  unit,
  title = 123,
}) => {
  const config = {
    width: width || 500,
    height: height || 500,
    autoFit: autoFit || true,
    data: {
      target: target || 0,
      total: total || 100,
      name: name || "score",
      // 动态计算 thresholds
      thresholds: [0, 0.2, 0.7, 1].map((item) => item * (total || 100)),
    },
    // 注意：Gauge 组件不直接支持 legend 属性
    // 如果需要图例，建议使用外部组件或 annotations
    scale: {
      color: {
        range: ["#F4664A", "#FAAD14", "green"],
      },
    },
    label: {
      show: true,
      formatter: ({ percent }) => `${(percent * 100).toFixed(0)}%`,
      position: "outside",
      fontSize: 20,
      fontWeight: "bold",
    },
    style: {
      textContent: (target, total) => `当前：${target}\n单位：${unit}`,
    },
  };

  return (
    <div
      style={{
        display: "flex", // 启用 Flexbox 布局
        flexDirection: "column", // 垂直排列子元素
        alignItems: "center", // 水平居中
        justifyContent: "center", // 垂直居中（可选，根据需要）
        margin: "0 auto",
        marginTop: "100px",
        width: "100%", // 根据需要调整宽度
        maxWidth: "500px", // 设置最大宽度
      }}
    >
      {title && (
        <h2
          style={{
            textAlign: "center", // 标题文本居中
            margin: "0 0 16px 0", // 调整标题与图表的间距
            fontSize: "24px",
            fontWeight: "bold",
          }}
        >
          {title}
        </h2>
      )}
      <Gauge {...config} />
    </div>
  );
};

export default CustomGauge;
