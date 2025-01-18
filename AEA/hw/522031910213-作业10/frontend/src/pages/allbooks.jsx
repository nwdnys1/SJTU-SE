import React, { useEffect, useState } from "react";
import { searchBooksByTags, searchBooks } from "../services/bookService";
import BookDisplay from "../components/book_display";
import { BasicLayout, LoginLayout } from "../components/layout";
import { Flex } from "antd";
import { useSearchParams } from "react-router-dom";
import CategoryBar from "../components/category";
import { defaultPage, defaultPageSize } from "../utils/config";

const AllBooksPage = () => {
  //作为父组件 初始渲染时 获取url参数 并进行搜索 但是不进行url参数的修改 只有子组件的回调函数才会修改url参数
  const [books, setBooks] = useState([]);
  const [length, setLength] = useState(0); // 用于分页
  const [searchParams, setSearchParams] = useSearchParams();
  const keyword = searchParams.get("keyword") || "";
  const page = searchParams.get("page") || defaultPage;
  const pageSize = searchParams.get("pageSize") || defaultPageSize; //默认为12 对应block布局
  const tags = searchParams.getAll("tag") || [];
  useEffect(() => {
    console.log(tags);
    if (keyword) {
      searchBooks({
        keyword: keyword,
        page: page,
        pageSize: pageSize,
      }).then((res) => {
        setBooks(res.content);
        setLength(res.total);
      });
    } else if (tags.length) {
      searchBooksByTags({
        tags: tags,
        page: page,
        pageSize: pageSize,
      }).then((res) => {
        setBooks(res.content);
        setLength(res.total);
      });
    } else
      searchBooks({
        keyword: "",
        page: page,
        pageSize: pageSize,
      }).then((res) => {
        setBooks(res.content);
        setLength(res.total);
      });
  }, [searchParams]);

  return (
    <BasicLayout>
      <Flex vertical align="center" style={{ marginTop: 20 }}>
        <CategoryBar />
        <BookDisplay books={books} withButton={false} length={length} />
      </Flex>
    </BasicLayout>
  );
};

export default AllBooksPage;
