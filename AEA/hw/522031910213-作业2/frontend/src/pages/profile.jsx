import { useEffect } from "react";
import { checkAuth } from "../services/loginService";
import { Profile } from "../components/user_profile";
import { BasicLayout, LoginLayout } from "../components/layout";

export const ProfilePage = () => {
  useEffect(() => {
    checkAuth();
  }, []);
  return (
    <LoginLayout>
      <Profile />
    </LoginLayout>
  );
};
