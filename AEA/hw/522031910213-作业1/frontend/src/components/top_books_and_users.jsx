import { useEffect, useState } from "react";
import { getTopBooks } from "../services/bookService";
import { Link, useSearchParams } from "react-router-dom";
import { DatePicker } from "antd";
import { Column } from "@ant-design/plots";
import { getTopUsers } from "../services/userService";
import { AdminLayout } from "./layout";
const { RangePicker } = DatePicker;
const TopBooksAndUsers = () => {
  const [topBooks, setTopBooks] = useState([]);
  const [topUsers, setTopUsers] = useState([]);
  const [searchParams, setSearchParams] = useSearchParams();
  const start = searchParams.get("start") || "1970-01-01 00:00:00";
  const end = searchParams.get("end") || "2100-01-01 00:00:00";
  const handleRange = (dates) => {
    const start = dates
      ? dates[0].format("YYYY-MM-DD hh:mm:ss")
      : "1970-01-01 00:00:00";
    const end = dates
      ? dates[1].format("YYYY-MM-DD" + " 23:59:59")
      : "2100-01-01 00:00:00";
    searchParams.set("start", start);
    searchParams.set("end", end);
    searchParams.set("page", 1);
    setSearchParams(searchParams);
  };
  const bookConfig = {
    data: topBooks,
    xField: "title", // 横坐标字段为书的标题
    yField: "sales", // 纵坐标字段为销售量
    marginTop: 50,
    height: 600,
    label: {
      text: (book) => `${book.sales}`, // 显示的文本内容
      textBaseline: "bottom",
    },
    style: {
      // 圆角样式
      radiusTopLeft: 10,
      radiusTopRight: 10,
      maxWidth: 30,
    },
    scrollbar: {
      x: {
        ratio: 0.5,
      },
    },
    title: "最受欢迎的书籍",
  };
  const userConfig = {
    data: topUsers,
    xField: "username",
    yField: "totalSpent",
    marginTop: 50,
    height: 400,
    label: {
      text: (user) => `${user.totalSpent}￥`,
      textBaseline: "bottom",
    },
    style: {
      radiusTopLeft: 10,
      radiusTopRight: 10,
      maxWidth: 30,
    },
 
    title: "最热衷购书的用户",
  };
  useEffect(() => {
    getTopBooks({
      start: start,
      end: end,
    }).then((res) => setTopBooks(res));
    getTopUsers({
      start: start,
      end: end,
    }).then((res) => setTopUsers(res));
  }, [searchParams]);

  return (
    <AdminLayout>
      <RangePicker
        onChange={handleRange}
        // defaultValue={[
        //   dayjs(start, "YYYY-MM-DD hh:mm:ss"),
        //   dayjs(end, "YYYY-MM-DD hh:mm:ss"),
        // ]}
        // defaultPickerValue={
        //   [
        //     dayjs(),
        //     dayjs(),
        //   ]
        // }
      />
      <Column {...bookConfig} />
      <Column {...userConfig} />
    </AdminLayout>
  );
};

export default TopBooksAndUsers;
