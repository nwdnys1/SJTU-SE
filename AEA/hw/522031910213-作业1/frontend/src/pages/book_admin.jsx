import { useEffect, useState } from "react";
import { AdminLayout } from "../components/layout";
import {
  addBook,
  deleteBookById,
  getAllBooks,
  searchBooks,
  updateBook,
} from "../services/bookService";
import {
  Button,
  Form,
  Image,
  Input,
  Pagination,
  Row,
  Space,
  Table,
  Upload,
} from "antd";
import { Column } from "@ant-design/plots";
import { BASEURL } from "../services/requestService";
import AddBookModal from "../components/addBookModal";
import EditBookModal from "../components/editBookModal";
import { useSearchParams } from "react-router-dom";
const BookAdminPage = () => {
  const [form] = Form.useForm();
  const [books, setBooks] = useState([]);
  const [length, setLength] = useState(0); // 用于分页
  const [editingBook, setEditingBook] = useState(null);
  const [isAdding, setIsAdding] = useState(false);
  const [searchParams, setSearchParams] = useSearchParams();
  const keyword = searchParams.get("keyword") || "";
  const page = searchParams.get("page") || 1;
  const pageSize = searchParams.get("pageSize") || 4; //默认为12 对应block布局
  const handlePageChange = (page, pageSize) => {
    setSearchParams({
      page: page,
      pageSize: pageSize,
      keyword: searchParams.get("keyword") || "",
    });
  };
  useEffect(() => {
    searchBooks({
      keyword: keyword,
      page: page,
      pageSize: pageSize,
    }).then((res) => {
      setBooks(res.content);
      setLength(res.total);
    });
  }, [keyword, page, pageSize]);
  const handleCover = (info, bookId) => {
    if (info.file.status === "done") {
      if (info.file.response.code === 200)
        setBooks(
          books.map((item) =>
            item.id === bookId
              ? { ...item, cover: info.file.response.data }
              : item
          )
        );
    }
  };
  const handleSearch = (value) => {
    setSearchParams({ keyword: value, page: 1, pageSize: pageSize });
  };
  const handleDelete = (id) => {
    deleteBookById(id)
      .then((res) => {
        setBooks(books.filter((item) => item.id !== id));
        alert("删除成功");
      })
      .catch((e) => alert(e));
  };
  const handleEdit = (book) => {
    setEditingBook(book);
    form.setFieldsValue(book);
  };
  const handleUpdate = (values) => {
    const book = {
      id: editingBook.id,
      ...values,
    };
    updateBook(book)
      .then((res) => {
        setBooks(books.map((item) => (item.id === book.id ? res : item)));
        setEditingBook(null);
        alert("更新成功");
      })
      .catch((e) => alert(e));
  };
  const handleAdd = (book) => {
    addBook(book)
      .then((res) => {
        setBooks([...books, res]);
        setIsAdding(false);
        alert("添加成功");
      })
      .catch((e) => alert(e));
  };
  return (
    <AdminLayout>
      <Row justify={"space-between"}>
        <Button onClick={() => setIsAdding(true)}>添加书籍</Button>
        <Input.Search
          onSearch={handleSearch}
          placeholder="搜索书籍"
          style={{ width: 200, marginBottom: 20 }}
        />
      </Row>
      <Table
        dataSource={books.map((item) => ({
          ...item,
          key: item.id,
        }))}
        pagination={{
          current: searchParams.get("page") || 1,
          defaultPageSize: 4,
          pageSize: searchParams.get("pageSize") || 4,
          onChange: handlePageChange,
          showQuickJumper: true,
          showSizeChanger: true,
          pageSizeOptions: ["4", "6", "12", "24"],
          total: length,
          showTotal: (total, range) =>
            `${total} 项中的 ${range[0]}-${range[1]} 项 `,
          position: "bottom",
        }}
      >
        <Column
          title="书名"
          dataIndex="title"
          key="title"
          render={(title, item) => <a href={`/details/${item.id}`}>{title}</a>}
        />
        <Column title="作者" dataIndex="author" key="author" />
        <Column
          title="价格"
          dataIndex="price"
          key="price"
          render={(price) => <span>￥{price}</span>}
        />
        <Column title="库存" dataIndex="stock" key="stock" />
        <Column title="ISBN" dataIndex="isbn" key="isbn" />
        <Column
          title="封面"
          dataIndex="cover"
          key="cover"
          render={(cover) => (
            <Image
              src={cover}
              alt="cover"
              style={{ width: 75, height: 100, objectFit: "cover" }}
            />
          )}
        />
        <Column
          title="操作"
          key="action"
          render={(item) => (
            <Space direction="vertical" align="center">
              <Button
                type="primary"
                onClick={() => {
                  handleDelete(item.id);
                }}
              >
                删除书籍
              </Button>
              <Button type="primary" onClick={() => handleEdit(item)}>
                编辑信息
              </Button>
              <Upload
                showUploadList={false}
                name="file"
                action={`${BASEURL}/api/book/cover/${item.id}`}
                withCredentials
                onChange={(info) => handleCover(info, item.id)}
              >
                <Button>上传封面</Button>
              </Upload>
            </Space>
          )}
        />
      </Table>
      <AddBookModal
        showModal={isAdding}
        handleCancel={() => setIsAdding(false)}
        handleSubmit={handleAdd}
      />
      <EditBookModal
        form={form}
        showModal={!!editingBook}
        handleCancel={() => setEditingBook(null)}
        handleSubmit={handleUpdate}
      />
    </AdminLayout>
  );
};

export default BookAdminPage;
