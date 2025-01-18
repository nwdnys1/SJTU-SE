import { useEffect, useState } from "react";
import { LoginLayout } from "./layout";
import { useSearchParams } from "react-router-dom";
import { DatePicker, Row } from "antd";
import { Bar, Column } from "@ant-design/plots";
import { getPurchasedBooks } from "../services/userService";
const { RangePicker } = DatePicker;
const UserStastics = () => {
  const [books, setBooks] = useState([]);
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
  const config = {
    data: books,
    xField: "title", // 横坐标字段为书的标题
    yField: "count", // 纵坐标字段为购买数量
    marginTop: 50,
    height: 400,
    sort: {
      reverse: true,
    },
    label: {
      text: (book) => `${book.count}`, // 显示的文本内容
      style: {
        textAnchor: (d) => (+d.frequency > 0.008 ? "right" : "start"),
        fill: (d) => (+d.frequency > 0.008 ? "#fff" : "#000"),
        dx: (d) => (+d.frequency > 0.008 ? -5 : 5),
      },
    },
    style: {
      // 圆角样式
      radiusTopLeft: 10,
      radiusTopRight: 10,
      maxWidth: 30,
    },

    title: "您购买过的书籍",
  };
  useEffect(() => {
    getPurchasedBooks({
      start: start,
      end: end,
    }).then((res) => setBooks(res));
  }, [searchParams]);
  return (
    <LoginLayout>
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
      <Bar {...config} />
      <Row justify={"space-between"}>
        <h2>{`总购书数：${books.reduce((acc, cur) => acc + cur.count, 0)}`}</h2>
        <h2>{`总花费：${books.reduce(
          (acc, cur) => acc + cur.price * cur.count,
          0
        )}￥`}</h2>
      </Row>
    </LoginLayout>
  );
};

export default UserStastics;
