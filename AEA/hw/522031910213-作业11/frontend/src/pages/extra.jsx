import { useState } from "react";
import SearchBox from "../components/searchbox";
import { BasicLayout, LoginLayout } from "../components/layout";
import { Flex, List } from "antd";
import { searchAuthorsByTitle } from "../services/bookService";
import { getRelatedTags } from "../services/tagService";
import { defaultPage, defaultPageSize } from "../utils/config";
import client from "../components/apollo_client";
import { gql } from "@apollo/client";
import BookCard from "../components/book_card";

const ExtraPage = () => {
  const [authors, setAuthors] = useState([]);
  const [books, setBooks] = useState([]);

  const handleSearchAuthor = (value) => {
    searchAuthorsByTitle(value)
      .then((res) => {
        setAuthors(res);
      })
      .catch((e) => alert(e));
  };
  const handleSearchTag = (value) => {
    getRelatedTags(value)
      .then((tags) => {
        if (tags.length === 0) {
          alert("No tag found.");
          return;
        }
        const tagsParams = tags.map((tag) => `tag=${tag}`).join("&");
        location.href = `/allbooks/list?${tagsParams}&page=${defaultPage}&pageSize=${defaultPageSize}`;
      })
      .catch((e) => alert(e));
  };
  const handleSearchKeyword = (value) => {
    const GET_BOOKS = gql`
      query {
        findBooksByTitle(title: "${value}") {
          id
          title
          author
          cover
          price
          bookDetails{
            rating
          }
        }
      }
    `;
    client
      .query({
        query: GET_BOOKS,
      })
      .then((res) => {
        setBooks(res.data.findBooksByTitle);
      })
      .catch((e) => alert(e));
  };

  return (
    <BasicLayout>
      <Flex vertical align="center">
        <h1>
          You can search authors by book's title here (with spring function):
        </h1>
        <SearchBox handleSearch={handleSearchAuthor} />
        {authors.map((author, index) => (
          <div key={index}>{author}</div>
        ))}
        <h1>You can search related books by the tag here (with neo4j):</h1>
        <SearchBox handleSearch={handleSearchTag} />
        <h1>
          You can search related books by the keyword here (with graphql):
        </h1>
        <SearchBox handleSearch={handleSearchKeyword} />
        <List
          dataSource={books}
          renderItem={(book) => <BookCard book={book} />}
          grid={{ gutter: 16, column: 4 }}
        ></List>
      </Flex>
    </BasicLayout>
  );
};

export default ExtraPage;
