import React from "react";
import { Layout } from "antd";
const { Content } = Layout;
export const BasicLayout = ({ children }) => {
  return (
    <Layout
      style={{ minWidth: "1000px", background: "white", minHeight: "100vh" }}
    >
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
    </Layout>
  );
};
