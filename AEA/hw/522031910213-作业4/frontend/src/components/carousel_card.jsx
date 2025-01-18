import { Card } from "antd";
import { Link } from "react-router-dom";

const CarouselCard = ({ book }) => {
  return (
    <Link to={`/details/${book.id}`}>
      <Card
        styles={{
          body: {
            display: "none",
          },
        }}
        cover={
          <img
            src={book.cover}
            alt={book.title}
            style={{
              aspectRatio: "4/3",
              objectFit: "cover",
              borderRadius: "5px",
            }}
          />
        }
      />
    </Link>
  );
};

export default CarouselCard;
