import React, { useState, useEffect } from "react";
import BasicLayout from "../layout/basicLayout";
import DeviceTag from "../components/deviceTag";
import DeviceTable from "../components/deviceTable";
import { getAllDevices, getDeviceData } from "../service/deviceService";
import { useSearchParams } from "react-router-dom";
import { Typography } from "antd";

const { Title } = Typography;

export default function DevicePage() {
  const [searchParams, setSearchParams] = useSearchParams();
  const [tagsData, setTagsData] = useState([]);
  const [tag, setTag] = useState(searchParams.get("device_name") || "");
  const [deviceData, setDeviceData] = useState([]);
  const [isLoading, setIsLoading] = useState(false); // 添加 isLoading 状态
  const [month, setMonth] = useState(searchParams.get("month") || "");

  // 初始化标签数据
  useEffect(() => {
    fetchAllTags();
  }, []);

  // 监听 searchParams 中 device_name 的变化
  useEffect(() => {
    const currentTag = searchParams.get("device_name") || "";
    setTag(currentTag);

    const currentMonth = searchParams.get("month") || "";
    setMonth(currentMonth);

    // 开始加载时设置 isLoading 为 true
    setIsLoading(true);
    console.log("Tag changed to: ", currentTag);

    // 根据 device_name 获取设备数据
    fetchDeviceData(currentTag, currentMonth)
      .then(() => {
        // 数据加载完成后设置 isLoading 为 false
        setIsLoading(false);
      })
      .catch((error) => {
        console.error("Error fetching device data:", error);
        setIsLoading(false); // 即使出错也要停止加载状态
      });
  }, [searchParams.get("device_name"), searchParams.get("month")]); // 监听 device_name 和 month 的变化

  // 获取所有标签
  const fetchAllTags = async () => {
    try {
      let res = await getAllDevices();
      setTagsData(res);
    } catch (error) {
      console.error("Error fetching tags:", error);
    }
  };

  // 获取设备数据
  const fetchDeviceData = async (deviceName, month) => {
    try {
      let res = await getDeviceData(deviceName, month);
      setDeviceData(res);
    } catch (error) {
      console.error("Error fetching device data:", error);
    }
  };

  return (
    <BasicLayout>
      <Title level={2} style={{ textAlign: "center", marginTop: "20px" }}>
        设备级数据查询展示界面
      </Title>
      <DeviceTag tags={tagsData} />
      {/* 将 isLoading 传递给 DeviceTable */}
      <DeviceTable data={deviceData} isLoading={isLoading} />
    </BasicLayout>
  );
}
