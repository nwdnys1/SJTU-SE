import { Button, Row, Space, Table } from "antd";
import { AdminLayout } from "../components/layout";
import { Column } from "@ant-design/plots";
import { useEffect, useState } from "react";
import { disableUser, enableUser, getAllUsers } from "../services/userService";

const UserAdminPage = () => {
  const [users, setUsers] = useState([]);
  useEffect(() => {
    getAllUsers().then((res) => {
      setUsers(res);
    });
  }, []);
  const handleEnabled = async (id) => {
    await enableUser(id)
      .then((res) => {
        setUsers(
          users.map((item) => {
            if (item.id === id) return { ...item, enabled: true };
            else return item;
          })
        );
        alert("用户已启用");
      })
      .catch((e) => alert(e));
  };
  const handleDisabled = async (id) => {
    await disableUser(id)
      .then((res) => {
        setUsers(
          users.map((item) => {
            if (item.id === id) return { ...item, enabled: false };
            else return item;
          })
        );
        alert("用户已禁用");
      })
      .catch((e) => alert(e));
  };
  return (
    <AdminLayout>
      <Table
        dataSource={users.map((item) => ({
          ...item,
          key: item.id,
        }))}
      >
        <Column title="ID" dataIndex="id" key="id" />
        <Column title="用户名" dataIndex="username" key="username" />
        <Column title="邮箱" dataIndex="email" key="email" />
        <Column title="联系方式" dataIndex="tel" key="tel" />
        <Column
          title="操作"
          key="action"
          render={(item) => (
            <Space>
              <Button
                disabled={!item.enabled}
                onClick={() => {
                  handleDisabled(item.id);
                }}
              >
                禁用
              </Button>
              <Button
                disabled={item.enabled}
                type="primary"
                onClick={() => {
                  handleEnabled(item.id);
                }}
              >
                启用
              </Button>
            </Space>
          )}
        />
      </Table>
    </AdminLayout>
  );
};

export default UserAdminPage;
