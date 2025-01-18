import React, { useEffect } from "react";
import { Layout } from "antd";
import MyFooter from "./footer";
import Navbar from "./navbar";
import { checkAuth } from "../services/loginService";
import { getUser } from "../services/userService";
import { useAuth } from "../context/authContext";
const { Content } = Layout;
export const BasicLayout = ({ children }) => {
  return (
    <Layout
      style={{ minWidth: "1000px", background: "white", minHeight: "100vh" }}
    >
      <Navbar />
      <Content
        style={{
          alignSelf: "center",
          margin: "20px 0",
          width: "65%",
          minWidth: 800,
        }}
      >
        {children}
      </Content>
      <MyFooter />
    </Layout>
  );
};
export const LoginLayout = ({ children }) => {
  const {setUser} = useAuth();
  useEffect(() => {
    checkAuth().catch((err) => {
      setUser(null);
      alert(err);
      location.href = "/login";
    });
  }, []);
  return (
    <Layout
      style={{ minWidth: "1000px", background: "white", minHeight: "100vh" }}
    >
      <Navbar />
      <Content
        style={{
          alignSelf: "center",
          margin: "20px 0",
          width: "65%",
          minWidth: 800,
        }}
      >
        {children}
      </Content>
      <MyFooter />
    </Layout>
  );
};
export const AdminLayout = ({ children }) => {
  useEffect(() => {
    checkAuth()
      .then((res) => {
        return res;
      })
      .then((res) => {
        if (res) {
          getUser().then((res) => {
            if (res.role !== "admin") {
              alert("您没有权限访问此页面");
              location.href = "/";
            }
          });
        }
      });
  }, []);
  return (
    <Layout
      style={{ minWidth: "1000px", background: "white", minHeight: "100vh" }}
    >
      <Navbar />
      <Content
        style={{
          alignSelf: "center",
          margin: "20px 0",
          width: "65%",
          minWidth: 800,
        }}
      >
        {children}
      </Content>
      <MyFooter />
    </Layout>
  );
};
