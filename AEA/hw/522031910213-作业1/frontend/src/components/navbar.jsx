import React from "react";
import { Menu, Layout, Row } from "antd";
import { Link } from "react-router-dom";
import {
  BookOutlined,
  HomeOutlined,
  ReadOutlined,
  BarChartOutlined,
} from "@ant-design/icons";
import SearchBox from "./searchbox";
import UserMenu from "./user_menu";
const { Header } = Layout;

const Items = [
  { key: "logo", icon: <BookOutlined />, label: "BOOKSTORE", link: "/" },
  { key: "home", icon: <HomeOutlined />, label: "首页", link: "/" },
  {
    key: "allBooks",
    icon: <ReadOutlined />,
    label: "所有书籍",
    link: "/allbooks/list",
  },

  {
    key: "stastics",
    icon: <BarChartOutlined />,
    label: "统计",
    link: "/stastics",
  },
];

const navItems = Items.map((item) => ({
  key: item.key,
  icon: item.icon,
  label: <Link to={item.link}>{item.label}</Link>,
}));

const handleSearch = (value) => {
  location.href = `/allbooks/list?page=1&keyword=${value}`;
};

const Navbar = () => {
  return (
    <Header
      style={{
        backgroundColor: "#f5f5f5",
        position: "sticky",
        top: 0,
        zIndex: 999,
        padding: "0",
      }}
    >
      <Row justify={"space-evenly"}>
        <Menu
          mode="horizontal"
          style={{
            width: "450px",
            backgroundColor: "#f5f5f5",
          }}
          items={navItems}
        ></Menu>

        <Row align="middle">
          <SearchBox handleSearch={handleSearch}/>
          <UserMenu />
        </Row>
      </Row>
    </Header>
  );
};

export default Navbar;
