import React from "react";
import { Line } from "@ant-design/plots";
import { format } from "fecha";

export default function TableTest() {
  const config = {
    data: {
      type: "fetch",
      value:
        "https://render.alipay.com/p/yuyan/180020010001215413/antd-charts/line-slider.json",
    },
    xField: (d) => new Date(d.date),
    yField: "close",
    axis: { x: { title: false, size: 40 }, y: { title: false, size: 36 } },
    slider: {
      x: { labelFormatter: (d) => format(d, "YYYY/M/D") },
      y: { labelFormatter: "~s" },
    },
  };
  return <Line {...config} />;
}
