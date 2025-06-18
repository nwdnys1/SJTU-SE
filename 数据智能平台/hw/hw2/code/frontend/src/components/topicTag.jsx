import React from "react";
import { Card, Tag } from "antd";
import { useSearchParams } from "react-router-dom";

export default function TopicTag({ tags }) {
  const [searchParams, setSearchParams] = useSearchParams();
  const [selectedTags, setSelectedTags] = React.useState(
    searchParams.getAll("topic") || [] // 支持多选时使用 getAll
  );

  // 关键词分类
  const categories = {
    温度: [],
    量: [],
    时间: [],
    压: [],
    功: [],
    其他: [],
  };

  // 将标签按关键词分类
  tags.forEach((tag) => {
    const tagName = tag || ""; // 假设 tag 是一个对象，包含 name 属性
    if (tagName.includes("温度")) {
      categories.温度.push(tag);
    } else if (tagName.includes("量")) {
      categories.量.push(tag);
    } else if (tagName.includes("时间")) {
      categories.时间.push(tag);
    } else if (tagName.includes("压")) {
      categories.压.push(tag);
    } else if (tagName.includes("功")) {
      categories.功.push(tag);
    } else {
      categories.其他.push(tag); // 不包含前面关键词的标签归类到“其他”
    }
  });

  const handleTagClick = (tag) => {
    if (!tag || !tag) return; // 确保 tag 和 tag 存在

    const newSelectedTags = selectedTags.includes(tag)
      ? selectedTags.filter((t) => t !== tag) // 取消选中
      : [tag]; // 选中当前标签（单选逻辑）

    setSelectedTags(newSelectedTags);

    setSearchParams((prevParams) => {
      const params = new URLSearchParams(prevParams);
      if (newSelectedTags.length > 0) {
        params.set("topic", newSelectedTags.join(",")); // 设置 topic 参数（多选时用逗号分隔）
      } else {
        params.delete("topic"); // 如果没有选中标签，删除 topic 参数
      }
      return params;
    });
  };

  return (
    <Card
      title="主题标签"
      style={{ width: "80%", margin: "0 auto", marginTop: "16px" }}
    >
      {/* 渲染每个分类 */}
      {Object.keys(categories).map((category) => (
        <div key={category}>
          <div
            style={{
              marginBottom: "8px",
              fontWeight: "bold",
              marginTop: "8px",
            }}
          >
            {category}
          </div>
          {categories[category].map((tag, index) => (
            <Tag.CheckableTag
              key={index} // 使用 index 作为 key 的兜底方案，确保唯一性
              checked={selectedTags.includes(tag)} // 判断是否选中
              onChange={() => handleTagClick(tag)}
            >
              {tag || "未命名标签"} {/* 确保标签内容有值 */}
            </Tag.CheckableTag>
          ))}
        </div>
      ))}
    </Card>
  );
}
