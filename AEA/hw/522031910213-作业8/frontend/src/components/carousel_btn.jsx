import { LeftOutlined, RightOutlined } from "@ant-design/icons";
import { Button, Row } from "antd";

const CarouselButton = ({ handlePrev, handleNext }) => {
  return (
    <Row
      justify={"end"}
      style={{
        width: "100%",
        position: "absolute",
        bottom: 4,
        zIndex: 100,
        right: 5,
      }}
    >
      <Button
        onClick={handlePrev}
        style={{
          opacity: "0.6",
        }}
        icon={<LeftOutlined />}
      />
      <div style={{ width: "5px" }} />
      <Button
        onClick={handleNext}
        style={{
          opacity: "0.6",
        }}
        icon={<RightOutlined />}
      />
    </Row>
  );
};

export default CarouselButton;
