import React, { useState, useEffect } from "react";
import BasicLayout from "../layout/basicLayout";
import CustomGauge from "../components/customGauge";
import ReportList from "../components/reportList";
import { getRealTimeData } from "../service/systemService";

export default function SystemPage() {
  const [realTimeData, setRealTimeData] = useState(null);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState(null);

  const [currentTime, setCurrentTime] = useState(new Date());

  // 获取实时数据
  useEffect(() => {
    const interval = setInterval(() => {
      fetchRealTimeData();
    }, 1000); // 每10秒获取一次数据

    return () => clearInterval(interval); // 清除定时器
  }, []);

  // 每秒更新当前时间
  useEffect(() => {
    const timer = setInterval(() => {
      setCurrentTime(new Date());
    }, 1000);

    return () => clearInterval(timer); // 清除定时器
  }, []);

  // 格式化时间显示
  const formatTime = (date) => {
    return date.toLocaleTimeString(); // 可根据需要调整时间格式
  };

  // 获取实时数据
  const fetchRealTimeData = async () => {
    try {
      const res = await getRealTimeData();
      console.log("实时数据:", res);
      setRealTimeData(res);
      setLoading(false);
    } catch (error) {
      console.error("Error fetching real-time data:", error);
      setError(error.message);
      setLoading(false);
    }
  };

  return (
    <BasicLayout>
      <h1
        style={{
          fontSize: "28px",
          marginBottom: "20px",
          marginTop: "50px",
          textAlign: "center",
        }}
      >
        当前时间: {formatTime(currentTime)}
      </h1>

      <div
        style={{
          display: "flex",
          justifyContent: "space-between",
          gap: "10px",
          flexWrap: "wrap", // 如果屏幕小，可以换行
        }}
      >
        {realTimeData?.["2号站三联供"] && (
          <CustomGauge
            title={"2号站三联供总供热"}
            unit={"kWh"}
            total={30000}
            target={realTimeData["2号站三联供"]["热总能量计累计热量"]}
          />
        )}
        {realTimeData?.["2号站三联供"] && (
          <CustomGauge
            title={"2号站三联供总供冷"}
            unit={"kWh"}
            total={5000}
            target={realTimeData["2号站三联供"]["总冷累计能量（kWh）"]}
          />
        )}
        {realTimeData?.["2号站热水供水总管"] && (
          <CustomGauge
            title={"2号站热水供水总管温度"}
            unit={"℃"}
            total={100}
            target={realTimeData["2号站热水供水总管"]["温度"]}
          />
        )}
        {realTimeData?.["2号站冷水供水总管"] && (
          <CustomGauge
            title={"2号站冷水供水总管温度"}
            unit={"℃"}
            total={100}
            target={realTimeData["2号站冷水供水总管"]["温度(℃)"]}
          />
        )}
      </div>
      <ReportList />
    </BasicLayout>
  );
}
