import React from "react";
import { Avatar, Button, Col, Row, Space, Tag, Typography, Upload } from "antd";
import { UploadOutlined } from "@ant-design/icons";
import { BASEURL } from "../services/requestService";
const { Text, Title, Paragraph } = Typography;

const ProfileInfo = ({ user, handleClick, setUser }) => {
  const handleChange = (info) => {
    if (info.file.status === "done") {
      if (info.file.response.code === 200)
        setUser({ ...user, avatar: info.file.response.data });
      else alert(info.file.response.message);
    } else if (info.file.status === "error") {
    }
  };
  return user ? (
    <Row style={{ width: "65%", margin: "0 auto", minWidth: 800 }}>
      <Space direction="vertical" align="center">
        <Avatar src={user.avatar} size={200} />
        <Upload
          showUploadList={false}
          name="avatar"
          action={`${BASEURL}/api/user/avatar`}
          withCredentials
          onChange={handleChange}
        >
          <Button icon={<UploadOutlined />}>上传头像</Button>
        </Upload>
        <Title level={2} style={{ margin: "10px 0 0 0" }}>
          {user.username}
        </Title>
        <Space>
          <Text type="secondary" strong>
            {user.role === "admin" ? "管理员" : "普通用户"}
          </Text>
          <Tag color={"gold"}>{`Lv.${user.level}`}</Tag>
        </Space>
        <Text strong> {`TEL：${user.tel}`}</Text>
        <Text strong underline>
          {" "}
          {`${user.email}`}
        </Text>
      </Space>
      <Col flex={"1"}>
        <Row justify={"space-between"} align={"middle"}>
          <Paragraph
            strong
            style={{ fontSize: "1.5em", margin: "30px 0 30px 40px" }}
          >
            {`个人简介:`}
          </Paragraph>
          <Button onClick={handleClick}>修改资料</Button>
        </Row>
        <Paragraph
          style={{
            fontSize: "1.2em",
            margin: "30px 0 0 40px",
            textIndent: "2em",
          }}
        >
          {user.aboutMe}
        </Paragraph>
      </Col>
    </Row>
  ) : null; //防止渲染时user为空
};

export default ProfileInfo;
