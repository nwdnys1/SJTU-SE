import React from "react";
import { Row, Col, Result, Button } from "antd";
import { Link } from "react-router-dom";

// 404页面
export default function ErrorPage() {
  return (
    <Row justify="center" align="middle" style={{ height: "100vh" }}>
      <Col span={12}>
        {/* 返回详情 */}
        <Result
          status="404"
          title="404"
          subTitle="Sorry, the page you visited does not exist."
          extra={
            <Link to="/">
              <Button type="primary">Back Home</Button>
            </Link>
          }
        />
      </Col>
    </Row>
  );
}
