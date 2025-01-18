import { useEffect, useState } from "react";

import { BasicLayout, LoginLayout } from "../components/layout";
import ProfileInfo from "../components/profile_info";
import { getOtherUser } from "../services/userService";
import { useParams } from "react-router-dom";

export const UserPage = () => {
  const [user, setUser] = useState({});
  const { username } = useParams();
  useEffect(() => {
    getOtherUser(username).then((res) => {
      setUser(res);
    });
  }, []);
  return (
    <LoginLayout>
      <ProfileInfo user={user} />
    </LoginLayout>
  );
};
