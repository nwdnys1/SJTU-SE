import { Card, Flex } from "antd";
import { BasicLayout } from "../components/layout";
import { useEffect, useState } from "react";
import G6graph from "../components/G6graph";
import Stats from "../components/stats";

function HomePage() {
  const apiInfo = [
    {
      name: "Skills",
      description: "查询具有某些技能的候选人",
      path: "/skills",
    },
    {
      name: "Jobs",
      description: "查询具有某些工作经历的候选人",
      path: "/jobs",
    },
  ];
  const [data, setData] = useState({});
  useEffect(() => {
    fetch("http://localhost:5000/graph")
      .then((res) => res.json())
      .then((data) => {
        setData(data);
      });
  }, []);
  return (
    <BasicLayout>
      <h1>知识图谱查询接口</h1>
      <Flex align="center" style={{ marginTop: 20 }}>
        {apiInfo.map((api, index) => (
          <Card
            key={index}
            title={api.name}
            style={{ width: 300, margin: 10 }}
            extra={<a href={api.path}>查看</a>}
          >
            <p>{api.description}</p>
          </Card>
        ))}
      </Flex>
      <h1>拥有相同技能的候选人们：</h1>
      <G6graph data={data} />
      <h1>候选人信息统计图：</h1>
      <Stats />
    </BasicLayout>
  );
}

export default HomePage;
