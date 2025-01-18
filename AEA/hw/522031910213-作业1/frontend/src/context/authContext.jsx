import React, { createContext, useContext, useEffect, useState } from "react";

// 创建一个上下文对象
const AuthContext = createContext();

// 创建一个 AuthProvider 组件来提供登录状态
export const AuthProvider = ({ children }) => {
  const [user, setUser] = useState(null);

  // 在组件加载时从 localStorage 中加载用户状态
  useEffect(() => {
    const savedUser = localStorage.getItem("user");
    if (savedUser) {
      setUser(JSON.parse(savedUser));
    }
  }, []);

  // 在用户状态变化时保存到 localStorage 中
  useEffect(() => {
    localStorage.setItem("user", JSON.stringify(user));
  }, [user]);

  return (
    <AuthContext.Provider value={{ user, setUser }}>
      {children}
    </AuthContext.Provider>
  );
};

export const useAuth = () => useContext(AuthContext);
