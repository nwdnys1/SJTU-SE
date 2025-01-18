import React from "react";
import ReactDOM from "react-dom/client"; // Import createRoot from "react-dom/client" instead of "react-dom"
import { ConfigProvider } from "antd";
import zhCN from "antd/lib/locale/zh_CN";
import HomePage from "./pages/home";
import AllBooksPage from "./pages/allbooks";
import DetailsPage from "./pages/details";
import ErrorPage from "./pages/error";
import OrdersPage from "./pages/orders";
import StasticsPage from "./pages/stastics";
import CartPage from "./pages/cart";
import { AuthProvider } from "./context/authContext";
import ScrollToTop from "./components/scrollToTop";
import { BrowserRouter as Router, Routes, Route } from "react-router-dom";
import { UserPage } from "./pages/user";
import UserAdminPage from "./pages/user_admin";
import BookAdminPage from "./pages/book_admin";
import OrderAdminPage from "./pages/order_admin";
import LoginPage from "./pages/login";
import ExtraPage from "./pages/extra";


const App = () => (
  <ConfigProvider locale={zhCN}>
    <AuthProvider>
      <Router>
        <ScrollToTop />
        <Routes>
          <Route path="/" element={<HomePage />} />
          <Route path="/login" element={<LoginPage />} />
          <Route path="/allbooks/list" element={<AllBooksPage />} />
          <Route path="/allbooks/category" element={<AllBooksPage />} />
          <Route path="/details/:id" element={<DetailsPage />} />
          <Route path="/cart" element={<CartPage />} />
          <Route path="/orders" element={<OrdersPage />} />
          <Route path="/stastics" element={<StasticsPage />} />
          <Route path="/user/:uid" element={<UserPage />} />
          <Route path="/admin/users" element={<UserAdminPage />} />
          <Route path="/admin/books" element={<BookAdminPage />} />
          <Route path="/admin/orders" element={<OrderAdminPage />} />
          <Route path="/extra" element={<ExtraPage />} />
          <Route path="*" element={<ErrorPage />} />
        </Routes>
      </Router>
    </AuthProvider>
  </ConfigProvider>
);

ReactDOM.createRoot(document.getElementById("root")).render(<App />);
