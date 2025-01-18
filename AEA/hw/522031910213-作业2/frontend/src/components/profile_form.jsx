import { Avatar, Button, Col, Form, Input, Row, Select } from "antd";
const { Option } = Select;
export const ProfileForm = ({ user, handleSave, handleCancel }) => {
  const [form] = Form.useForm();
  form.setFieldsValue(user);
  return (
    <Form
      form={form}
      onFinish={handleSave}
      layout="vertical"
      initialValues={{ role: "admin", level: "gold" }}
      style={{
        width: "60%",
        margin: "0 auto",
        minWidth: 800,
      }}
    >
      <Row justify={"space-evenly"} align={"middle"}>
        <Avatar size={200} src={user.avatar} />
        <Col style={{ width: 500 }}>
          <Row justify={"space-between"}>
            <Form.Item
              name="username"
              label="用户名"
              rules={[{ required: true, message: "请输入您的用户名!" }]}
            >
              <Input
                size="large"
                style={{ width: 150 }}
                allowClear
                maxLength={10}
              />
            </Form.Item>
            <Form.Item name="role" label="权限">
              <Select size="large" disabled style={{ width: 150 }}>
                <Option value="admin">管理员</Option>
                <Option value="user">普通用户</Option>
              </Select>
            </Form.Item>
            <Form.Item name="level" label="等级">
              <Select size="large" disabled style={{ width: 150 }}>
                <Option value="gold">Lv.1</Option>
                <Option value="silver">Lv.2</Option>
                <Option value="bronze">Lv.3</Option>
              </Select>
            </Form.Item>
          </Row>
          <Row justify={"space-between"}>
            <Form.Item name="email" label="邮箱">
              <Input size="large" allowClear maxLength={30} />
            </Form.Item>
            <Form.Item name="tel" label="联系方式">
              <Input size="large" allowClear maxLength={30} />
            </Form.Item>
          </Row>
        </Col>
      </Row>
      <Form.Item name="aboutMe" label="个人简介">
        <Input.TextArea
          size="large"
          rows={10}
          style={{ width: "100%" }}
          maxLength={3000}
          allowClear
        />
      </Form.Item>
      <Row justify={"center"}>
        <Button type="primary" size="large" htmlType="submit">
          保存
        </Button>
        <div style={{ width: 20 }} />
        <Button onClick={handleCancel} size="large">
          取消
        </Button>
        <div style={{ width: 20 }} />
        <Button size="large">清空</Button>
      </Row>
    </Form>
  );
};
