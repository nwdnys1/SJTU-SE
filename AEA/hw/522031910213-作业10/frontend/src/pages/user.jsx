import { useEffect, useState } from "react";

import { BasicLayout, LoginLayout } from "../components/layout";
import ProfileInfo from "../components/profile_info";
import { getOtherUser } from "../services/userService";
import { useParams } from "react-router-dom";
import { Profile } from "../components/user_profile";

export const UserPage = () => {
  const [user, setUser] = useState({});
  const { uid } = useParams();
  useEffect(() => {
    getOtherUser(uid).then((res) => {
      setUser(res);
    });
  }, []);
  return (
    <BasicLayout>
      <Profile />
    </BasicLayout>
  );
};
