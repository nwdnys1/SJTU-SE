import { Button, Form, Input, Flex, Row } from "antd";
import React from "react";
import { placeOrder } from "../services/orderService";

const { TextArea } = Input;
export default function OrderModal({ selectedItems, totalPrice }) {
  const handleSubmit = async ({ address, receiver, tel }) => {
    if (!address || !receiver || !tel) {
      alert("请填写完整信息！");
      return;
    }
    let orderInfo = {
      address,
      receiver,
      tel,
      cids: selectedItems.map((item) => item.id),
    };
    await placeOrder(orderInfo);
    alert("订单提交成功！正在处理中...");
    window.location.reload();
  };
  return (
    <Form layout="vertical" preserve={false} onFinish={handleSubmit}>
      <Flex vertical justify="space-around" style={{ width: 300, height: 400 }}>
        <Form.Item
          name="receiver"
          label="收货人"
          required
          style={{ width: "100%", margin: 0 }}
        >
          <Input />
        </Form.Item>
        <Form.Item
          name="tel"
          label="联系电话"
          required
          style={{ width: "100%", margin: 0 }}
        >
          <Input />
        </Form.Item>
        <Form.Item
          name="address"
          label="收货地址"
          required
          style={{ width: "100%", margin: 0 }}
        >
          <TextArea rows={4} />
        </Form.Item>
        <Form.Item>
          <Row justify="end" align={"middle"}>
            <p>合计：￥{totalPrice}</p>
            <Button type="primary" htmlType="submit">
              提交订单
            </Button>
          </Row>
        </Form.Item>
      </Flex>
    </Form>
  );
}
