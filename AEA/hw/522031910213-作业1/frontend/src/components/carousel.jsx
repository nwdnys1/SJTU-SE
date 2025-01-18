import React from "react";
import { Carousel } from "antd";
import CarouselButton from "./carousel_btn";
import CarouselCard from "./carousel_card";
import Blur from "./blur";

const BookCarousel = ({ books }) => {
  const carouselRef = React.createRef();
  return (
    <div
      style={{
        maxWidth: "600px",
        width: "50%",
        position: "relative",
      }}
    >
      <Carousel effect="fade" ref={carouselRef} autoplay>
        {books.map((book) => (
          <CarouselCard key={book.id} book={book} />
        ))}
      </Carousel>
      <CarouselButton
        handlePrev={() => {
          carouselRef.current.prev();
        }}
        handleNext={() => {
          carouselRef.current.next();
        }}
      />
      <Blur />
    </div>
  );
};

export default BookCarousel;
