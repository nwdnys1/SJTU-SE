import React from "react";
import { Layout, Space } from "antd";
import NavBar from "../components/navbar";

const { Header, Content, Footer } = Layout;

export default function BasicLayout({ children }) {
  return (
    <Layout className="basic-layout" style={{ minHeight: "100vh" }}>
      <Header
        className="header"
        style={{ position: "sticky", width: "100%", zIndex: 1000 }}
      >
        <NavBar />
      </Header>
      <Content>{children}</Content>
      <Footer className="footer">
        <Space direction="vertical">
          <div>DataPlatform ©2025</div>
        </Space>
      </Footer>
    </Layout>
  );
}
