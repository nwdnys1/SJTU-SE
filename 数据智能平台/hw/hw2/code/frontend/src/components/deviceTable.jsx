import React from "react";
import { Spin } from "antd"; // 使用 Ant Design 的 Spin 组件显示加载状态
import { Line } from "@ant-design/plots";

export default function DeviceTable({ data, isLoading }) {
  const config = {
    //data: data?.slice(0, 6) || [], // 取前10000条数据
    data: data,
    xField: "timestamp",
    yField: "value",
    slider: {
      x: {},
      y: {},
    },
    colorField: "category",
    interactions: [{ type: "slider-x" }], // 启用滑块交互
  };

  return (
    <div
      style={{
        width: "80%",
        margin: "0 auto",
        marginTop: "16px",
      }}
    >
      {isLoading ? (
        // 显示加载中的 Spin 组件
        <Spin size="large" style={{ display: "block", marginTop: "20px" }} />
      ) : (
        // 数据加载完成后显示图表
        <Line {...config} />
      )}
    </div>
  );
}
