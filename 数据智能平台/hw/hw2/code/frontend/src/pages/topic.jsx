import React, { useState, useEffect } from "react";
import BasicLayout from "../layout/basicLayout";
import DeviceTag from "../components/deviceTag";
import DeviceTable from "../components/deviceTable";
import { getAllDevices, getDeviceData } from "../service/deviceService";
import { useSearchParams } from "react-router-dom";
import { Typography } from "antd";
import { getAllTopics, getDevicesByTopic } from "../service/topicService";
import TopicTag from "../components/topicTag"; // 引入 TopicTag 组件

const { Title } = Typography;

export default function TopicPage() {
  const [searchParams, setSearchParams] = useSearchParams();
  const [tagsData, setTagsData] = useState([]);
  const [tag, setTag] = useState(searchParams.get("device_name") || "");
  const [deviceData, setDeviceData] = useState([]);
  const [isLoading, setIsLoading] = useState(false); // 添加 isLoading 状态
  const [month, setMonth] = useState(searchParams.get("month") || "");
  const [topicData, setTopicData] = useState([]); // 添加 topicData 状态
  const [topic, setTopic] = useState(searchParams.get("topic") || ""); // 添加 topic 状态

  // 初始化标签数据
  useEffect(() => {
    fetchAllTopics(); // 获取所有主题数据
  }, []);

  // 监听 searchParams 中 device_name 的变化
  useEffect(() => {
    const currentTopic = searchParams.get("topic") || ""; // 获取当前主题
    setTopic(currentTopic); // 设置当前主题

    const currentTag = searchParams.get("device_name") || "";
    setTag(currentTag);

    const currentMonth = searchParams.get("month") || "";
    setMonth(currentMonth);

    // 开始加载时设置 isLoading 为 true
    setIsLoading(true);
    console.log("Tag changed to: ", currentTag);

    // 如果 topic不存在，则不进行数据加载
    if (!currentTopic) {
      return;
    }

    // 根据 topic 获取设备名
    fetchAllTags(currentTopic)
      .then(() => {})
      .catch((error) => {
        console.error("Error fetching tags:", error);
      });

    //如果currentTag或currentMonth不存在，则不进行数据加载
    if (!currentTag || !currentMonth) {
      return;
    }

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
  }, [
    searchParams.get("device_name"),
    searchParams.get("month"),
    searchParams.get("topic"),
  ]); // 监听 device_name 和 month 的变化

  // 获取所有标签
  const fetchAllTags = async (topic) => {
    try {
      let res = await getDevicesByTopic(topic); // 使用 topic 获取设备数据
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

  // 获取所有主题
  const fetchAllTopics = async () => {
    try {
      let res = await getAllTopics();
      console.log("主题数据:", res);
      setTopicData(res);
    } catch (error) {
      console.error("Error fetching topics:", error);
    }
  };

  return (
    <BasicLayout>
      <Title level={2} style={{ textAlign: "center", marginTop: "20px" }}>
        主题级数据查询展示界面
      </Title>
      <TopicTag tags={topicData} />
      <DeviceTag tags={tagsData} />
      {/* 将 isLoading 传递给 DeviceTable */}
      <DeviceTable data={deviceData} isLoading={isLoading} />
    </BasicLayout>
  );
}
