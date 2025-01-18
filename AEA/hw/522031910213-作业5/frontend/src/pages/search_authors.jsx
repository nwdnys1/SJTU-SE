import { useState } from "react";
import SearchBox from "../components/searchbox";
import { LoginLayout } from "../components/layout";
import { Flex } from "antd";
import { searchAuthorsByTitle } from "../services/bookService";

const SearchAuthorsPage = () => {
  const [authors, setAuthors] = useState([]);
  const handleSearch = (value) => {
    searchAuthorsByTitle(value)
      .then((res) => {
        setAuthors(res);
      })
      .catch((e) => alert(e));
  };
  return (
    <LoginLayout>
      <Flex vertical align="center">
        <SearchBox handleSearch={handleSearch} />
        <h1>You can search authors by book's title here:</h1>
        {authors.map((author,index) => (
          <div key={index}>{author}</div>
        ))}
      </Flex>
    </LoginLayout>
  );
};

export default SearchAuthorsPage;
