import React from "react";
import { Layout } from "antd";

const { Footer } = Layout;

const MyFooter = () => {
  return (
    <Footer
      style={{
        textAlign: "center",
      }}
    >
      <p>© 2024 E-BOOKSTORE</p>
      <a href="https://nwdnys1.github.io/about/">关于作者 </a>
    </Footer>
  );
};

export default MyFooter;
