import {
  Button,
  Col,
  Flex,
  Image,
  InputNumber,
  Modal,
  Row,
  Table,
  Typography,
} from "antd";
import { useEffect, useState, useRef } from "react";
import {
  deleteCartItem,
  getCartItems,
  updateCartItemNumber,
} from "../services/cartService";
import { Link } from "react-router-dom";
import { DeleteOutlined } from "@ant-design/icons";
import OrderModal from "./order_modal";
import { orderWSURL } from "../services/requestService";
import { computePrice } from "../services/orderService";
const { Text } = Typography;
const CartTable = () => {
  const [items, setItems] = useState(null);
  const [showModal, setShowModal] = useState(false);
  const [selectedItems, setSelectedItems] = useState([]);
  const [totalPrice, setTotalPrice] = useState(0);
  const ws = useRef(null);
  const initWebSocket = () => {
    const socket = new WebSocket(orderWSURL);

    socket.onopen = () => {
      console.log("Connected to WebSocket server");
      ws.current = socket; // 保存WebSocket对象
    };

    socket.onmessage = (e) => {
      alert(e.data);
    };

    socket.onerror = (err) => {
      console.error("WebSocket error:", err);
    };

    socket.onclose = () => {
      console.log("Disconnected from WebSocket server");
    };
  };

  const handleDeleteItem = async (id) => {
    await deleteCartItem(id);
    setItems(items.filter((item) => item.id !== id));
    setSelectedItems(selectedItems.filter((item) => item.id !== id));
  };

  useEffect(() => {
    initWebSocket();
    getCartItems().then((res) => {
      setItems(res);
    });
    return () => {
      ws.current?.close();
    };
  }, []);
  useEffect(() => {
    computePrice(selectedItems)
      .then((res) => {
        setTotalPrice(res);
      })
      .catch((e) => {
        alert(e);
      });
  }, [selectedItems]);

  const openModal = () => {
    setShowModal(true);
  };

  const closeModal = () => {
    setShowModal(false);
  };

  const handleNumberChange = async (id, number) => {
    await updateCartItemNumber(id, number);
    items.filter((item) => item.id === id)[0].quantity = number;
    let selected = selectedItems.find((item) => item.id === id);
    if (selected) {
      selected.quantity = number;
      setSelectedItems([...selectedItems]);
    }
    setItems([...items]);
  };

  const columns = [
    {
      title: "书名",
      dataIndex: "book",
      key: "book_title",
      render: (book) => <Link to={`/details/${book.id}`}>{book.title}</Link>,
    },
    {
      title: "数量",
      dataIndex: "number",
      key: "number",
      render: (number, item) => (
        <InputNumber
          min={1}
          max={item.book.bookDetails.stock}
          value={item.quantity}
          onChange={(newNumber) => {
            handleNumberChange(item.id, newNumber);
          }}
        />
      ),
    },
    {
      title: "价格",
      dataIndex: "book",
      key: "book_price",
      render: (book) => book.price + "元",
    },
    {
      title: "操作",
      dataIndex: "",
      key: "action",
      render: (item) => (
        <DeleteOutlined
          type="primary"
          onClick={() => {
            handleDeleteItem(item.id);
          }}
        />
      ),
    },
  ];

  return (
    items && (
      <Flex vertical justify="center">
        {showModal && (
          <Modal
            open={showModal}
            onCancel={closeModal}
            footer={null}
            centered
            keyboard
            width={"auto"}
          >
            <OrderModal selectedItems={selectedItems} totalPrice={totalPrice} />
          </Modal>
        )}
        <Table
          columns={columns}
          rowSelection={{
            onChange: (_, selectedItems) => {
              setSelectedItems(selectedItems);
            },
          }}
          expandable={{
            expandedRowRender: (item) => (
              <Row justify={"space-between"} gutter={8}>
                <Col span={3}>
                  <Image src={item.book.cover} style={{ objectFit: "cover" }} />
                </Col>
                <Col span={20}>
                  <Text> {item.book.description} </Text>
                  <br />
                  <Text> 作者：{item.book.author} </Text>
                  <br />
                  <Text> 库存：{item.book.stock} </Text>
                  <br />
                  <Text> ISBN: {item.book.isbn} </Text>
                </Col>
              </Row>
            ),
          }}
          dataSource={items.map((item) => ({
            ...item,
            key: item.id,
          }))}
        />
        <Row justify="end" align={"middle"}>
          <p>合计：￥{totalPrice}</p>
          <div style={{ width: 10 }} />
          <Button
            type="primary"
            disabled={selectedItems.length === 0}
            onClick={openModal}
          >
            立刻下单
          </Button>
        </Row>
      </Flex>
    )
  );
};

export default CartTable;
