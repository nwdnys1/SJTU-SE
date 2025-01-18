import { PayCircleOutlined, ShoppingCartOutlined } from "@ant-design/icons";
import { Button, Divider, Flex, Row, Typography } from "antd";
import BookInfo from "./book_info";
import { addCartItem } from "../services/cartService";

const { Paragraph } = Typography;

export const BookDetails = ({ book }) => {
  const handleAddToCart = () => {
    addCartItem(book.id);
  };

  const handleBuyNow = () => {};
  return (
    book && (
      <Flex vertical>
        <BookInfo book={book} />
        <Divider style={{ margin: "5px 0" }} />
        <Row>
          <Paragraph
            style={{ textIndent: "2em" }}
            ellipsis={{
              rows: 4,
              expandable: true,
              symbol: "展开",
            }}
          >
            {book.description}
          </Paragraph>
        </Row>
        <Divider style={{ margin: "10px 0" }} />
        <Row justify={"end"}>
          <Button
            type="primary"
            icon={<ShoppingCartOutlined />}
            onClick={handleAddToCart}
            style={{ marginRight: "10px" }}
          >
            加入购物车
          </Button>
          <Button
            type="primary"
            icon={<PayCircleOutlined />}
            onClick={handleBuyNow}
          >
            立即购买
          </Button>
        </Row>
      </Flex>
    )
  );
};
