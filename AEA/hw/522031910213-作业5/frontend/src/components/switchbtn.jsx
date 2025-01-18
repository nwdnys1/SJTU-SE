import React from "react";
import { Segmented } from "antd";
import { AppstoreOutlined, BarsOutlined } from "@ant-design/icons";

const SwitchButton = ({ handleChange, layout }) => {
  return (
    <Segmented
      value={layout}
      defaultValue="block"
      onChange={handleChange}
      size="large"
      options={[
        {
          value: "block",
          icon: <AppstoreOutlined />,
        },
        {
          value: "list",
          icon: <BarsOutlined />,
        },
      ]}
    />
  );
};

export default SwitchButton;
