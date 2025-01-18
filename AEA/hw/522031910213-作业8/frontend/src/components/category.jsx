import { Row, Tag } from "antd";
import { useEffect, useState } from "react";
import { useSearchParams } from "react-router-dom";
import { getTags } from "../services/tagService";
import { defaultPageSize } from "../utils/config";

const CategoryBar = () => {
  //这个子组件的回调是修改tag 其他都不变 初始化时获取tags 并且如果url参数改变了tag 则选中tag
  const [searchParams, setSearchParams] = useSearchParams();
  const [tags, setTags] = useState([]);
  const [selected, setSelected] = useState([]);
  useEffect(() => {
    getTags()
      .then((res) => {
        setTags(res);
      })
      .catch((e) => alert(e));
    setSelected(searchParams.getAll("tag"));
  }, [searchParams]);
  const handleChange = (tag, checked) => {
    let newSelected = [];
    if (checked) {
      newSelected = [...selected, tag];
    } else {
      newSelected = selected.filter((t) => t !== tag);
    }
    setSelected(newSelected);
    setSearchParams({
      page: 1,
      pageSize: searchParams.get("pageSize") || defaultPageSize,
      keyword: "",
      tag: newSelected,
      layout: searchParams.get("layout") || "block",
    });
  };
  return (
    tags && (
      <Row align={"middle"} style={{ marginBottom: 20 }}>
        {tags.map((tag) => (
          <Tag.CheckableTag
            key={tag.id}
            checked={selected.includes(tag.id.toString())}
            onChange={(checked) => handleChange(tag.id.toString(), checked)}
          >
            {tag.name}
          </Tag.CheckableTag>
        ))}
      </Row>
    )
  );
};

export default CategoryBar;
