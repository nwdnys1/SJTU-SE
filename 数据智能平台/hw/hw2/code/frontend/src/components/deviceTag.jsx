import React from "react";
import { Card, Tag, Space } from "antd";
import { Input, Button, Typography, Alert } from "antd";
import { useSearchParams } from "react-router-dom";

const { Title, Text } = Typography;

export default function DeviceTag({ tags }) {
  const [selectedTags, setSelectedTags] = React.useState([]);
  const [selectedMonth, setSelectedMonth] = React.useState("");
  const [searchParams, setSearchParams] = useSearchParams();
  const [tag, setTag] = React.useState(searchParams.get("device_name") || "");

  // 关键词分类
  const categories = {
    三联供: [],
    发电机: [],
    热: [],
    冷: [],
    锅炉: [],
    燃烧机: [],
  };

  // 将标签按关键词分类
  tags.forEach((tag) => {
    Object.keys(categories).forEach((category) => {
      if (tag.table_name && tag.table_name.includes(category)) {
        categories[category].push(tag);
      }
    });
  });

  const handleTagChange = (tag) => {
    const tagCategory = Object.keys(categories).find((category) =>
      tag.table_name.includes(category)
    );

    // 获取当前类别下的所有标签
    const tagsInSameCategory = categories[tagCategory] || [];

    // 更新选中的标签列表
    const updatedSelectedTags = selectedTags.includes(tag.table_name)
      ? selectedTags.filter((t) => t !== tag.table_name)
      : tagsInSameCategory.length > 0
      ? [tag.table_name]
      : [];

    setSelectedTags(updatedSelectedTags);
    setSearchParams((prevParams) => {
      const params = new URLSearchParams(prevParams);
      if (updatedSelectedTags.length > 0) {
        params.set("device_name", updatedSelectedTags.join(","));
      } else {
        params.delete("device_name");
      }
      return params;
    });
  };

  const handleMonthChange = (month) => {
    const newSelectedMonth = month === selectedMonth ? "" : month;
    setSelectedMonth(newSelectedMonth);
    setSearchParams((prevParams) => {
      const params = new URLSearchParams(prevParams);
      if (newSelectedMonth) {
        params.set("month", newSelectedMonth);
      } else {
        params.delete("month");
      }
      return params;
    });
  };

  const months = [
    { label: "2018年1月", value: "1" },
    { label: "2018年2月", value: "2" },
    { label: "2018年4月", value: "4" },
    { label: "2018年7月", value: "7" },
    { label: "2018年10月", value: "10" },
  ];

  return (
    <div style={{ width: "80%", margin: "0 auto", marginTop: "16px" }}>
      <Card title="设备名称" style={{ marginBottom: "16px" }}>
        <Space direction="vertical" style={{ width: "100%" }}>
          {Object.keys(categories).map((category) => (
            <div key={category}>
              <Text strong style={{ display: "block", marginBottom: "8px" }}>
                {category}
              </Text>
              {categories[category].map((tag) => (
                <Tag.CheckableTag
                  key={tag.table_name}
                  checked={selectedTags.includes(tag.table_name)}
                  onChange={() => handleTagChange(tag)}
                >
                  {tag.table_name}
                </Tag.CheckableTag>
              ))}
            </div>
          ))}
        </Space>
      </Card>

      <Card title="月份选择">
        <Space direction="horizontal" style={{ width: "100%" }}>
          {months.map((month) => (
            <Tag.CheckableTag
              key={month.value}
              checked={selectedMonth === month.value}
              onChange={() => handleMonthChange(month.value)}
            >
              {month.label}
            </Tag.CheckableTag>
          ))}
        </Space>
      </Card>
    </div>
  );
}
