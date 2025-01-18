import React from "react";
import { Card, Typography, List, Space, Col, Row } from "antd";
import {
  ShoppingCartOutlined,
  LikeOutlined,
  MessageOutlined,
} from "@ant-design/icons";
import { Link } from "react-router-dom";
import { addCartItem } from "../services/cartService";

const { Title, Paragraph } = Typography;

const IconText = ({ icon, text }) => (
  <Space>
    {React.createElement(icon)}
    {text}
  </Space>
);

const BookListItem = ({ book }) => {
  return (
    <List.Item>
      <Card
        style={{ width: "100%" }}
        styles={{
          body: {
            padding: "15px",
          },
        }}
        actions={[
          <ShoppingCartOutlined
            onClick={() => {
              addCartItem(book.id);
            }}
          />,
          <IconText
            icon={LikeOutlined}
            text="156"
            key="list-vertical-like-o"
          />,
          <IconText
            icon={MessageOutlined}
            text={book.bookDetails.comments.length}
            key="list-vertical-message"
          />,
        ]}
      >
        <Row>
          <Col flex={"1"}>
            <Title level={4} style={{ margin: "5px 0" }} title={book.title}>
              {book.title}
            </Title>
            <Title
              type="secondary"
              style={{ fontSize: "14px", margin: "5px 0" }}
            >
              {book.author}
            </Title>
            <Paragraph
              ellipsis={{ rows: 6, expandable: true, symbol: "展开" }}
              style={{
                fontSize: "16px",
                textIndent: "2em",
                margin: "5px 0",
              }}
            >
              {book.description}
            </Paragraph>
          </Col>
          <Link to={`/details/${book.id}`}>
            <img
              src={book.cover}
              alt="Book Cover"
              style={{
                alignSelf: "center",
                marginLeft: "30px",
                width: "150px",
                height: "200px",
                borderRadius: "10px",
                objectFit: "cover",
              }}
            />
          </Link>
        </Row>
      </Card>
    </List.Item>
  );
};

export default BookListItem;
