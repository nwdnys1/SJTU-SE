import React from "react";
import { Input } from "antd";
import { useSearchParams } from "react-router-dom";
const { Search } = Input;
const SearchBox = ({ handleSearch }) => {
  return (
    <Search
      placeholder="输入书名、作者或关键词搜索书籍"
      onSearch={handleSearch}
      style={{
        width: "300px",
      }}
    />
  );
};

export default SearchBox;
