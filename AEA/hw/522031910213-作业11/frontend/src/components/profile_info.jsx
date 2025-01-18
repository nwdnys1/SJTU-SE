import React, { useEffect, useState } from "react";
import { Avatar, Button, Col, Row, Space, Tag, Typography, Upload } from "antd";
import { UploadOutlined } from "@ant-design/icons";
import { BASEURL } from "../services/requestService";
import { useParams } from "react-router-dom";
import { getOtherUser } from "../services/userService";
import { useAuth } from "../context/authContext";
const { Text, Title, Paragraph } = Typography;

const ProfileInfo = ({ handleClick }) => {
  const { user, setUser } = useAuth();
  const { uid } = useParams();
  const [realUser, setRealUser] = useState(null);
  useEffect(() => {
    console.log(uid);
    console.log(user?.id);
    if (user?.id != uid) {
      getOtherUser(uid)
        .then((res) => {
          setRealUser(res);
        })
        .catch((e) => {
          alert(e);
          history.back();
        });
    } else {
      setRealUser(user);
    }
  }, [uid]);
  const handleChange = (info) => {};
  return (
    realUser && (
      <Row style={{ width: "65%", margin: "0 auto", minWidth: 800 }}>
        <Space direction="vertical" align="center">
          <Avatar src={realUser.avatar} size={200} />
          {uid == user?.id && (
            <Upload
              showUploadList={false}
              name="avatar"
              action={`${BASEURL}/api/user/avatar`}
              withCredentials
              onChange={handleChange}
            >
              <Button icon={<UploadOutlined />}>上传头像</Button>
            </Upload>
          )}
          <Title level={2} style={{ margin: "10px 0 0 0" }}>
            {realUser.username}
          </Title>
          <Space>
            <Text type="secondary" strong>
              {realUser.role === "admin" ? "管理员" : "普通用户"}
            </Text>
            <Tag color={"gold"}>{`Lv.${realUser.level}`}</Tag>
          </Space>
          <Text strong> {`TEL：${realUser.tel}`}</Text>
          <Text strong underline>
            {" "}
            {`${realUser.email}`}
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
            {uid == user?.id && <Button onClick={handleClick}>修改资料</Button>}
          </Row>
          <Paragraph
            style={{
              fontSize: "1.2em",
              margin: "30px 0 0 40px",
              textIndent: "2em",
            }}
          >
            {realUser.aboutMe}
          </Paragraph>
        </Col>
      </Row>
    )
  );
};

export default ProfileInfo;
