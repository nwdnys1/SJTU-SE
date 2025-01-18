import React from "react";
import { Form, Input, Button, Typography, Flex, Row } from "antd";
import {
  UserOutlined,
  LockOutlined,
  EyeTwoTone,
  EyeInvisibleOutlined,
  MailOutlined,
} from "@ant-design/icons";
import { register } from "../services/loginService";
import { logoUri } from "../utils/config";

const { Title, Paragraph } = Typography;

const RegisterModal = () => {
  return (
    <Form initialValues={{ remember: true }} onFinish={register}>
      <Flex
        vertical
        align="center"
        justify="space-evenly"
        style={{ width: 400, height: 400 }}
      >
        <img alt="logo" src={logoUri} width="45px"></img>
        <Title level={2} style={{ margin: "0" }}>
          E-BOOKSTORE
        </Title>
        <Paragraph type="secondary" style={{ fontSize: 16, margin: "0px" }}>
          电子书城
        </Paragraph>
        <Form.Item
          id="username"
          name="username"
          rules={[{ required: true, message: "请输入您的用户名!" }]}
          style={{ width: "100%", margin: 0 }}
        >
          <Input
            size="large"
            prefix={<UserOutlined />}
            placeholder="用户名"
            allowClear
          />
        </Form.Item>
        <Form.Item
          id="password"
          name="password"
          rules={[{ required: true, message: "请输入您的密码!" }]}
          style={{ width: "100%", margin: 0 }}
        >
          <Input.Password
            size="large"
            prefix={<LockOutlined />}
            type="password"
            placeholder="密码"
            allowClear
            iconRender={(visible) =>
              visible ? <EyeTwoTone /> : <EyeInvisibleOutlined />
            }
          />
        </Form.Item>
        <Form.Item
          style={{ width: "100%", margin: 0 }}
          id="confirm_password"
          name="confirm_password"
          rules={[
            {
              required: true,
              message: "请再次输入您的密码!",
            },
            ({ getFieldValue }) => ({
              validator(_, value) {
                if (!value || getFieldValue("password") === value) {
                  return Promise.resolve();
                }
                return Promise.reject(new Error("两次输入的密码不一致!"));
              },
            }),
          ]}
        >
          <Input.Password
            size="large"
            prefix={<LockOutlined />}
            type="password"
            placeholder="确认密码"
            allowClear
            iconRender={(visible) =>
              visible ? <EyeTwoTone /> : <EyeInvisibleOutlined />
            }
          />
        </Form.Item>
        <Form.Item
          id="email"
          name="email"
          rules={[{ required: true, message: "请输入您的邮箱!" }]}
          style={{ width: "100%", margin: 0 }}
        >
          <Input
            type="email"
            size="large"
            prefix={<MailOutlined />}
            placeholder="邮箱"
            allowClear
          />
        </Form.Item>
        <Button type="primary" htmlType="submit" style={{ width: "100%" }}>
          注册
        </Button>
      </Flex>
    </Form>
  );
};

export default RegisterModal;
