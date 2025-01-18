import { DatePicker } from "antd";
import { useAuth } from "../context/authContext";
import TopBooksAndUsers from "../components/top_books_and_users";
import UserStastics from "../components/user_stastics";

const StasticsPage = () => {
  const { user } = useAuth();
  return user?.role === "admin" ? <TopBooksAndUsers /> : <UserStastics />;
};

export default StasticsPage;
