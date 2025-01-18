import React, { useContext, useState } from "react";
import {
  UserOutlined,
  ShoppingCartOutlined,
  OrderedListOutlined,
  LogoutOutlined,
  TeamOutlined,
  DatabaseOutlined,
} from "@ant-design/icons";
import { Avatar, Dropdown, Menu, Modal, Row } from "antd";
import { logout } from "../services/loginService";
import { Link } from "react-router-dom";
import LoginModal from "./login";
import { useAuth } from "../context/authContext";
import RegisterModal from "./register";

const UserMenu = () => {
  const [showModal, setShowModal] = useState(false);
  const { user, setUser } = useAuth();
  const [display, setDisplay] = useState("login");

  const logoutAndClearUser = () => {
    setUser(null);
    logout()
      .then((res) => {
        alert(res);
        location.href = "/";
      })
      .catch((e) => {
        alert(e);
      });
  };

  const showLogin = () => {
    if (!user) setShowModal(true);
  };
  const closeModal = () => {
    setShowModal(false);
  };
  const handleDisplay = () => {
    if (display == "login") {
      setDisplay("register");
    } else {
      setDisplay("login");
    }
  };

  const items = [
    {
      key: "userPage",
      label: <Link to={`/user/${user?.id}`}>个人中心</Link>,
      icon: <UserOutlined />,
    },
    {
      key: "logout",
      label: <div onClick={logoutAndClearUser}>退出登录</div>,
      icon: <LogoutOutlined />,
    },
    {
      key: "cart",
      label: <Link to="/cart">购物车</Link>,
      icon: <ShoppingCartOutlined />,
    },
    {
      key: "orders",
      label: <Link to="/orders">订单</Link>,
      icon: <OrderedListOutlined />,
    },
    {
      key: "userAdmin",
      label: <Link to="/admin/users">用户管理</Link>,
      icon: <TeamOutlined />,
    },
    {
      key: "bookAdmin",
      label: <Link to="/admin/books">书籍管理</Link>,
      icon: <DatabaseOutlined />,
    },
    {
      key: "orderAdmin",
      label: <Link to="/admin/orders">订单管理</Link>,
      icon: <OrderedListOutlined />,
    },
  ];
  return (
    <>
      {user ? (
        <Dropdown
          menu={{
            items: user.role === "admin" ? items : items.slice(0, 4),
          }}
        >
          <Menu
            mode="horizontal"
            style={{
              backgroundColor: "#f5f5f5",
              minWidth: "100px",
            }}
            items={[
              {
                key: "user",
                icon: <Avatar src={user.avatar} shape="round" />,
                label: user ? user.username : "登录",
                onClick: showLogin,
                style: { display: "flex", alignItems: "center" },
              },
            ]}
          />
        </Dropdown>
      ) : (
        <Menu
          mode="horizontal"
          style={{
            backgroundColor: "#f5f5f5",
            minWidth: "100px",
            justifyContent: "center",
          }}
          items={[
            {
              key: "user",
              icon: <UserOutlined />,
              label: user ? user.username : "登录",
              onClick: showLogin,
            },
          ]}
        />
      )}
      <Modal
        open={showModal}
        onCancel={closeModal}
        footer={null}
        centered
        keyboard
        width={"auto"}
      >
        {display == "login" ? <LoginModal /> : <RegisterModal />}
        <Row justify={"end"} style={{ alignSelf: "end" }}>
          {display == "login" ? (
            <div>
              没有账号？<a onClick={handleDisplay}>现在注册</a>
            </div>
          ) : (
            <div>
              已有账号？<a onClick={handleDisplay}>点击登录</a>
            </div>
          )}
        </Row>
      </Modal>
    </>
  );
};

export default UserMenu;
