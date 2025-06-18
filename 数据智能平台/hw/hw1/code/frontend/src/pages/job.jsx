import { Button, Col, Row, Space, Tag } from "antd";
import { useEffect, useState } from "react";
import { filterByJobs, getJobs } from "../service/service";
import { BasicLayout } from "../components/layout";
import { Link } from "react-router-dom";

function JobPage() {
  const [jobs, setJobs] = useState([]);
  const [selected, setSelected] = useState([]);
  const [people, setPeople] = useState([]);
  const [expand, setExpand] = useState(false);
  useEffect(() => {
    getJobs()
      .then((res) => {
        setJobs(res);
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
    filterByJobs(selected).then((res) => {
      setPeople(res);
    });
  };
  const handleExpand = () => {
    setExpand(!expand);
  };
  return (
    <BasicLayout>
      <h1>根据工作经历查询</h1>
      {jobs && (
        <Col align={"middle"} style={{ marginBottom: 30 }}>
          {expand
            ? jobs.map((job, index) => (
                <Tag.CheckableTag
                  key={index}
                  checked={selected.includes(job)}
                  onChange={(checked) => handleChange(job, checked)}
                >
                  {job}
                </Tag.CheckableTag>
              ))
            : jobs.slice(0, 30).map((job, index) => (
                <Tag.CheckableTag
                  key={index}
                  checked={selected.includes(job)}
                  onChange={(checked) => handleChange(job, checked)}
                >
                  {job}
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

export default JobPage;
