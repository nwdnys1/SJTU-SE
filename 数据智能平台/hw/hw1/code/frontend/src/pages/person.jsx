import React, { useEffect, useState } from "react";
import { Card, Row, Col } from "antd";
import { getPerson } from "../service/service";
import { BasicLayout } from "../components/layout";
import { Link, useParams } from "react-router-dom";
import { BASEURL } from "../service/requestService";

const PersonPage = () => {
  let { name } = useParams();
  const [person, setPerson] = useState({});

  useEffect(() => {
    getPerson(name).then((data) => {
      setPerson(data);
      console.log(data);
    });
  }, []);

  return (
    <BasicLayout>
      <h1>候选人信息</h1>
      <Card title={person.name}>
        候选人名字：{person.name}
        <br />
        技能：{person.skills?.map((skill) => skill).join(", ")}
        <br />
        工作经验：{person.jobs?.map((job) => job).join(", ")}
        <br />
        教育经历：{person.degrees?.map((degree) => degree).join(", ")}
        <br />
        项目经验：
        {person.projects?.map((project) => project).join(", ")}
        <br />
        现居地：{person.countries?.map((country) => country).join(", ")}
        <br />
        语言：{person.languages?.map((language) => language).join(", ")}
        <br />
        工作地区：{person.locations?.map((location) => location).join(", ")}
        <br />
        简历链接：<Link to={`${BASEURL}/pdf/${person.resume}`}>点击查看</Link>
      </Card>
    </BasicLayout>
  );
};

export default PersonPage;
