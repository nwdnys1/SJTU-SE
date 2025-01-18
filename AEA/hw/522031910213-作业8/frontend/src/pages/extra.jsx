import { useState } from "react";
import SearchBox from "../components/searchbox";
import { BasicLayout, LoginLayout } from "../components/layout";
import { Flex } from "antd";
import { searchAuthorsByTitle } from "../services/bookService";
import { getRelatedTags } from "../services/tagService";
import { defaultPage, defaultPageSize } from "../utils/config";

const ExtraPage = () => {
  const [authors, setAuthors] = useState([]);

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
  return (
    <BasicLayout>
      <Flex vertical align="center">
        <h1>You can search authors by book's title here:</h1>
        <SearchBox handleSearch={handleSearchAuthor} />
        {authors.map((author, index) => (
          <div key={index}>{author}</div>
        ))}
        <h1>You can search related books by the tag here:</h1>
        <SearchBox handleSearch={handleSearchTag} />
      </Flex>
    </BasicLayout>
  );
};

export default ExtraPage;
