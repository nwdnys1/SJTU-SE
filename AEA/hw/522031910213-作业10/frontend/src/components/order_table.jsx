import { DatePicker, Flex, Image, Input, List, Row, Table } from "antd";
import { computePrice, searchOrders } from "../services/orderService";
import { useEffect, useState } from "react";
import { Link, useSearchParams } from "react-router-dom";
import { defaultPageSize } from "../utils/config";
const { RangePicker } = DatePicker;

export default function OrderTable() {
  const [orders, setOrders] = useState([]);
  const [length, setLength] = useState(0); // 用于分页
  const [searchParams, setSearchParams] = useSearchParams();
  const keyword = searchParams.get("keyword") || "";
  const page = searchParams.get("page") || 1;
  const pageSize = searchParams.get("pageSize") || defaultPageSize;
  const start = searchParams.get("start") || "1970-01-01 00:00:00";
  const end = searchParams.get("end") || "2100-01-01 00:00:00";
  useEffect(() => {
    searchOrders({
      keyword: keyword,
      start: start,
      end: end,
      page: page,
      pageSize: pageSize,
    }).then((res) => {
      Promise.all(
        res.content.map(async (order) => {
          let totalPrice = await computePrice(order.orderItems);
          order.totalPrice = totalPrice;
          return order;
        })
      ).then((orders) => {
        setOrders(orders);
      });
      setLength(res.total);
    });
  }, [keyword, page, pageSize, start, end]);

  const handleSearch = (value) => {
    searchParams.set("keyword", value);
    searchParams.set("page", 1);
    setSearchParams(searchParams);
  };
  const handleRange = (dates) => {
    const start = dates[0].format("YYYY-MM-DD hh:mm:ss");
    const end = dates[1].format("YYYY-MM-DD" + " 23:59:59");
    searchParams.set("start", start);
    searchParams.set("end", end);
    searchParams.set("page", 1);
    setSearchParams(searchParams);
  };
  const handlePageChange = (page, pageSize) => {
    searchParams.set("page", page);
    searchParams.set("pageSize", pageSize);
    setSearchParams(searchParams);
  };
  const columns = [
    { title: "收货人", dataIndex: "receiver", key: "receiver" },
    { title: "联系方式", dataIndex: "tel", key: "tel" },
    { title: "收货地址", dataIndex: "address", key: "address" },
    {
      title: "总价",
      key: "totalPrice",
      render: (_, order) => `￥${order.totalPrice}`,
    },
    {
      title: "下单时间",
      dataIndex: "createTime",
      key: "createTime",
      render: (time) => new Date(time).toLocaleString(),
    },
  ];

  return (
    <Flex vertical justify="center">
      <Row justify={"space-between"} style={{ marginBottom: 20 }}>
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
        <Input.Search
          defaultValue={keyword}
          onSearch={handleSearch}
          placeholder="搜索订单"
          style={{ width: 200 }}
        />
      </Row>
      <Table
        columns={columns}
        expandable={{
          expandedRowRender: (order) => (
            <List
              dataSource={order.orderItems}
              renderItem={(item, _) => (
                <List.Item>
                  <List.Item.Meta
                    avatar={
                      <Image
                        src={item.book.cover}
                        width={75}
                        height={100}
                        style={{ objectFit: "cover" }}
                      />
                    }
                    title={
                      <Link to={`/details/${item.book.id}`}>
                        {item.book.title}
                      </Link>
                    }
                    description={`数量：${item.quantity} 价格：${item.book.price}元`}
                  />
                </List.Item>
              )}
            />
          ),
        }}
        dataSource={orders.map((order) => ({
          ...order,
          key: order.id,
        }))}
        pagination={{
          current: page,
          defaultPageSize: defaultPageSize,
          pageSize: pageSize,
          onChange: handlePageChange,
          showQuickJumper: true,
          showSizeChanger: true,
          pageSizeOptions: ["6", "12", "24", "48"],
          total: length,
          showTotal: (total, range) =>
            `${total} 项中的 ${range[0]}-${range[1]} 项 `,
          position: "bottom",
        }}
      />
    </Flex>
  );
}
