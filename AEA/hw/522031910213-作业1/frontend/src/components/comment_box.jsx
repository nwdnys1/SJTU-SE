import React, { useState } from "react";
import { Input, Button, Avatar, Rate, Row } from "antd";
import { addComment } from "../services/commentService";
import { useParams } from "react-router-dom";
import { useAuth } from "../context/authContext";
import { defaultAvatar } from "../utils/config";

const CommentBox = ({ setComments }) => {
  const [content, setContent] = useState("");
  const [rating, setRating] = useState(0);
  const { user, setUser } = useAuth();
  let { id } = useParams();

  const handleInputChange = (e) => {
    setContent(e.target.value);
  };
  const handleRatingChange = (value) => {
    setRating(value);
  };
  const handleSubmit = async () => {
    if (!content || !rating) {
      alert("请填写评论内容和评分");
      return;
    }
    if (content.length > 3000) {
      alert(`评论内容过长，当前字数${content.length}，请控制在3000字以内`);
      return;
    }
    await addComment({ bid: id, content: content })
      .then((res) => {
        setComments((comments) => [...comments, res]);
        setContent("");
        alert("评论成功！");
      })
      .catch((e) => {
        alert(e);
      });
  };

  return (
    <Row align={"middle"} justify={"space-evenly"}>
      <Avatar src={user ? user.avatar : defaultAvatar} size="large" />
      <div style={{ width: 20 }} />
      <Input.TextArea
        rows={2}
        placeholder="请输入您的评论"
        value={content}
        onChange={handleInputChange}
        style={{ flex: 1 }}
      />
      <div style={{ width: 20 }} />
      <Rate value={rating} onChange={handleRatingChange} />
      <div style={{ width: 20 }} />
      <Button type="primary" onClick={handleSubmit}>
        提交评论
      </Button>
    </Row>
  );
};

export default CommentBox;
