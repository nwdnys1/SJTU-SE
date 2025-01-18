import React, { useEffect, useState } from "react";
import { useParams } from "react-router-dom";
import { BookDetails } from "../components/book_details";
import { BookComments } from "../components/book_comments";
import { BasicLayout, LoginLayout } from "../components/layout";
import { getBookById } from "../services/bookService";

const DetailsPage = () => {
  let { id } = useParams();
  const [book, setBook] = useState(null);
  const [comments, setComments] = useState([]);
  useEffect(() => {
    getBookById(id).then((res) => {
      setBook(res);
      setComments(res.bookDetails.comments);
    });
  }, []);

  return (
    book && (
      <BasicLayout>
        <BookDetails book={book} />
        <BookComments comments={comments} setComments={setComments} />
      </BasicLayout>
    )
  );
};

export default DetailsPage;
