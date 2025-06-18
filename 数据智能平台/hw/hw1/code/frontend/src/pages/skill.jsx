import { Button, Col, Flex, Row, Space, Tag } from "antd";
import { useEffect, useState } from "react";
import { filterBySkills, getSkills } from "../service/service";
import { BasicLayout } from "../components/layout";
import { Link } from "react-router-dom";

function SkillPage() {
  const [skills, setSkills] = useState([]);
  const [selected, setSelected] = useState([]);
  const [people, setPeople] = useState([]);
  const [expand, setExpand] = useState(false);
  useEffect(() => {
    getSkills()
      .then((res) => {
        setSkills(res);
      })
      .catch((e) => alert(e));
  }, []);
  const handleChange = (skill, checked) => {
    let newSelected = [];
    if (checked) {
      newSelected = [...selected, skill];
    } else {
      newSelected = selected.filter((t) => t !== skill);
    }
    setSelected(newSelected);
  };
  const handleClick = () => {
    filterBySkills(selected).then((res) => {
      setPeople(res);
      console.log(res);
    });
  };
  const handleExpand = () => {
    setExpand(!expand);
  };
  return (
    <BasicLayout>
      <h1>根据技能查询</h1>
      {skills && (
        <Col align={"middle"} style={{ marginBottom: 30 }}>
          {expand
            ? skills.map((skill, index) => (
                <Tag.CheckableTag
                  key={index}
                  checked={selected.includes(skill)}
                  onChange={(checked) => handleChange(skill, checked)}
                >
                  {skill}
                </Tag.CheckableTag>
              ))
            : skills.slice(0, 30).map((skill, index) => (
                <Tag.CheckableTag
                  key={index}
                  checked={selected.includes(skill)}
                  onChange={(checked) => handleChange(skill, checked)}
                >
                  {skill}
                </Tag.CheckableTag>
              ))}
          <br />
          <br />
          <Space>
            <Button onClick={handleExpand}>{expand ? "收起" : "展开"}</Button>
            <Button type="primary" onClick={handleClick}>
              查询
            </Button>
          </Space>
        </Col>
      )}
      查询结果：
      <br />
      {people.map((person, index) => (
        <Link to={`/person/${person}`} key={index}>
          {person}
          <br />
        </Link>
      ))}
    </BasicLayout>
  );
}

export default SkillPage;
