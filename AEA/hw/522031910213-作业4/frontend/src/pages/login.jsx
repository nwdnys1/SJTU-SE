import { Flex, Row } from "antd";
import LoginModal from "../components/login";
import RegisterModal from "../components/register";
import { useState } from "react";
import { BasicLayout } from "../components/layout";

const LoginPage = () => {
  const [display, setDisplay] = useState("login");
  const handleDisplay = () => {
    if (display == "login") {
      setDisplay("register");
    } else {
      setDisplay("login");
    }
  };
  return (
    <BasicLayout>
      <Flex
        vertical
        align="flex-end"
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
      </Flex>
    </BasicLayout>
  );
};

export default LoginPage;
