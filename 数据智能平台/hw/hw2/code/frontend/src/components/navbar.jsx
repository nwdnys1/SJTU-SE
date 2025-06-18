import React, { useState, useContext, useEffect } from "react";
import { Link } from "react-router-dom";
import { Col, Menu, Row, Button, Input, Drawer } from "antd";
import {
  DatabaseOutlined,
  DeploymentUnitOutlined,
  ClusterOutlined,
  ToolOutlined,
  FundOutlined,
  AreaChartOutlined,
} from "@ant-design/icons";
import { useNavigate, useLocation, useSearchParams } from "react-router-dom";

const { Search } = Input;

export default function Navbar() {
  // 使用 useNavigate 和 useLocation 获取路由信息，使用 useSearchParams 获取 URL 参数
  const navigate = useNavigate();
  const location = useLocation();

  // 导航栏菜单项
  const navItems = [
    { label: "设备查询", value: "/", icon: <ToolOutlined /> },
    { label: "主题查询", value: "/topic", icon: <DatabaseOutlined /> },
    { label: "系统展示", value: "/system", icon: <ClusterOutlined /> },
    { label: "供能预测", value: "/predict", icon: <DeploymentUnitOutlined /> },
    { label: "PCA结果", value: "/pca", icon: <FundOutlined /> },
    // { label: "报表一览", value: "/report", icon: <AreaChartOutlined /> },
  ].filter(Boolean);

  // 生成导航栏菜单项
  const navMenuItems = navItems.map((item) => ({
    key: item.value,
    label: <Link to={item.value}>{item.label}</Link>,
    icon: item.icon,
  }));

  return (
    <>
      <Row
        className="navbar"
        justify="space-between"
        align="middle"
        style={{ height: "64px", boxShadow: "0 2px 8px rgba(0, 0, 0, 0.15)" }}
      >
        <Col style={{ width: "50%", minWidth: "870px" }}>
          <Row align="middle">
            <p style={{ color: "black" }}>分析结果展示</p>
            <div style={{ minWidth: "760px" }}>
              <Menu
                mode="horizontal"
                items={navMenuItems}
                selectedKeys={[location.pathname]}
              />
            </div>
          </Row>
        </Col>
      </Row>
    </>
  );
}
