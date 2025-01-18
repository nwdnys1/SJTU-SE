import React from "react";
import { Card, Button, Flex, Row } from "antd";
import { ShoppingCartOutlined } from "@ant-design/icons";

import { Typography } from "antd";
import { Link } from "react-router-dom";
import { addCartItem } from "../services/cartService";

const { Title } = Typography;

const BookCard = ({ book }) => {
  return (
    <Flex vertical>
      <Link to={`/details/${book.id}`}>
        <Card
          bordered={false}
          styles={{
            body: {
              display: "none",
            },
          }}
          cover={
            <>
              <img
                alt={book.title}
                src={book.cover}
                style={{
                  aspectRatio: "3/4",
                  objectFit: "cover",
                  borderRadius: "5px",
                  maxWidth: "300px",
                }}
              />
              <Button
                type="primary"
                icon={<ShoppingCartOutlined />}
                style={{
                  width: "30px",
                  position: "absolute",
                  left: 0,
                  bottom: 0,
                  zIndex: 100,
                }}
                onClick={(e) => {
                  e.stopPropagation();//
                  e.preventDefault();
                  addCartItem(book.id);
                }}
              />
              <Row
                style={{
                  width: "100%",
                  position: "absolute",
                  bottom: 3,
                  right: 5,
                  color: "orange",
                  fontSize: "20px",
                  textAlign: "right",
                  zIndex: 1,
                }}
              >
                {(book?.bookDetails.rating/10).toFixed(1)}
              </Row>
            </>
          }
        />
      </Link>
      <Flex vertical style={{ height: "80px", padding: 3 }}>
        <Link to={`/details/${book.id}`}>
          <Row>
            <Title
              style={{
                color: "blue",
                fontSize: "14px",
                margin: 0,
              }}
              ellipsis={{ rows: 2 }}
            >
              {book.title}
            </Title>
          </Row>
        </Link>
        <Row>
          <Title
            style={{
              fontSize: "12px",
              margin: "0",
            }}
            ellipsis={{ rows: 1 }}
          >
            {book.author}
          </Title>
        </Row>
        <Row>
          <span
            style={{
              fontSize: "14px",
              fontWeight: "bold",
              margin: 0,
            }}
          >
            {book?.price?.toFixed(2) + "¥"}
          </span>
        </Row>
      </Flex>
    </Flex>
  );
};

export default BookCard;
