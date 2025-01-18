import React, { useEffect, useState } from "react";
import { useParams } from "react-router-dom";
import { BookDetails } from "../components/book_details";
import { BookComments } from "../components/book_comments";
import { BasicLayout, LoginLayout } from "../components/layout";
import { getBookById } from "../services/bookService";

const DetailsPage = () => {
  let { id } = useParams();
  const [book, setBook] = useState(null);
  useEffect(() => {
    getBookById(id).then((book) => setBook(book));
  }, []);

  return (
    <LoginLayout>
      <BookDetails book={book} />
      <BookComments />
    </LoginLayout>
  );
};

export default DetailsPage;
