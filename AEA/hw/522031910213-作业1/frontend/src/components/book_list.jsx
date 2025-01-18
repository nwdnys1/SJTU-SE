import React from "react";
import { List, Pagination, Row } from "antd";
import BookListItem from "./book_listitem";
import { useSearchParams } from "react-router-dom";

export const ListLayout = ({ books, length, paging }) => {
  //这个子组件的回调是修改page和pageSize 其他都不变
  const [searchParams, setSearchParams] = useSearchParams();
  const handlePageChange = (page, pageSize) => {
    setSearchParams({
      page: page,
      pageSize: pageSize,
      keyword: searchParams.get("keyword") || "",
      tag: searchParams.get("tag") || "",
      layout: searchParams.get("layout") || "block",
    });
  };

  return (
    <List
      dataSource={books}
      renderItem={(book) => <BookListItem book={book} />}
    >
      <Row justify="center">
        {paging && (
          <Pagination
            current={searchParams.get("page") || 1}
            defaultPageSize={4}
            pageSize={searchParams.get("pageSize") || 4}
            onChange={handlePageChange}
            showQuickJumper
            showSizeChanger
            pageSizeOptions={["4", "6", "12", "24", "48"]}
            total={length}
            showTotal={(total, range) =>
              `${total} 项中的 ${range[0]}-${range[1]} 项 `
            }
            position="bottom"
          />
        )}
      </Row>
    </List>
  );
};
