import React, { useState } from "react";
import { ProfileForm } from "./profile_form";
import ProfileInfo from "./profile_info";
import { useAuth } from "../context/authContext";
import { updateUser } from "../services/userService";

export const Profile = () => {
  const [isEditing, setIsEditing] = useState(false);
  const { user, setUser } = useAuth();
  const handleEdit = () => {
    setIsEditing(true);
  };
  const handleCancel = () => {
    setIsEditing(false);
  };
  const handleSave = async (user) => {
    if (user.aboutMe.length > 3000) {
      alert(`个人简介过长，当前字数${user.aboutMe.length}，请控制在3000字以内`);
      return;
    }
    try {
      await updateUser(user).then((res) => {
        setUser(res);// 更新会话中的用户信息
        console.log(res);
        alert("修改成功！");
        setIsEditing(false);
      });
    } catch (e) {
      alert(e);
    }
  };

  return isEditing ? (
    <ProfileForm
      handleSave={handleSave}
      handleCancel={handleCancel}
      user={user}
    />
  ) : (
    <ProfileInfo user={user} handleClick={handleEdit} setUser={setUser} />
  );
};
