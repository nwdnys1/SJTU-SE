import { Button, Form, Input, Modal } from "antd";

const EditBookModal = ({ form, showModal, handleCancel, handleSubmit }) => {
  return (
    <Modal
      open={showModal}
      title="编辑书籍信息"
      onCancel={handleCancel}
      footer={null}
    >
      <Form form={form} onFinish={handleSubmit}>
        <Form.Item
          label="书名"
          name="title"
          rules={[{ required: true, message: "请输入书名" }]}
        >
          <Input />
        </Form.Item>
        <Form.Item
          label="作者"
          name="author"
          rules={[{ required: true, message: "请输入作者" }]}
        >
          <Input />
        </Form.Item>
        <Form.Item
          label="库存"
          name="stock"
          rules={[{ required: true, message: "请输入库存" }]}
        >
          <Input type="number" min={0}/>
        </Form.Item>
        <Form.Item
          label="ISBN"
          name="isbn"
          rules={[{ required: true, message: "请输入ISBN" }]}
        >
          <Input type="number" min={0}/>
        </Form.Item>
        <Form.Item>
          <Button type="primary" htmlType="submit">
            提交
          </Button>
        </Form.Item>
      </Form>
    </Modal>
  );
};

export default EditBookModal;
