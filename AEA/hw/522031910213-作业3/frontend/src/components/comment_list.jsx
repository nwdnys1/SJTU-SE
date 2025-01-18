import React, { useEffect, useState } from "react";
import { Card, Avatar, Button, List, Flex, Typography, Row, Space } from "antd";
import ReplyBox from "./reply_box";
import { Link } from "react-router-dom";
import { ReplyList } from "./reply_list";
import {
  CloseCircleOutlined,
  LikeOutlined,
  LikeTwoTone,
  MessageOutlined,
} from "@ant-design/icons";
import { getCids, likeComment } from "../services/likeService";
import { useAuth } from "../context/authContext";
const { Paragraph } = Typography;
const CommentList = ({ comments, setComments }) => {
  const { user, setUser } = useAuth();
  const [cids, setCids] = useState([]); //cids is an array of comment ids that the user has liked
  const [replying, setReplying] = useState(null);
  useEffect(() => {
    //get the comment ids that the user has liked
    if (user) {
      getCids().then((res) => {
        setCids(res);
      });
    }
  }, [user]);

  const handleReply = (id) => {
    setReplying(replying === id ? null : id);
  };

  const handleLike = (comment) => {
    const cid = comment.id;
    try {
      likeComment(cid).then((res) => {
        if (cids.includes(cid)) {
          setCids(cids.filter((c) => c !== cid));
          comment.likes = comment.likes.filter((l) => l.user.id !== user.id);
        } else {
          setCids([...cids, cid]);
          comment.likes.push({ user: { id: user.id } });
        }
      });
    } catch (e) {
      alert(e);
    }
  };
  return (
    <List
      dataSource={comments}
      renderItem={(comment) => (
        <List.Item>
          <Flex vertical style={{ width: "100%" }}>
            <Card
              style={{ margin: 0, width: "100%" }}
              actions={[
                <Space size={3} onClick={() => handleLike(comment)}>
                  {cids.includes(comment.id) ? (
                    <LikeTwoTone key="like" />
                  ) : (
                    <LikeOutlined key="like" />
                  )}
                  {` ${comment.likes.length}`}
                </Space>,
                !(replying === comment.id) ? (
                  <Space size={3} onClick={() => handleReply(comment.id)}>
                    <MessageOutlined key="like" />
                    回复
                  </Space>
                ) : (
                  <Space size={3} onClick={() => handleReply(comment.id)}>
                    <CloseCircleOutlined key="like" />
                    取消
                  </Space>
                ),
              ]}
            >
              <Card.Meta
                avatar={
                  <Link to={`/user/${comment.user.username}`}>
                    <Avatar src={comment.user.avatar} size="large" />
                  </Link>
                }
                title={comment.user.username}
                description={<Paragraph>{comment.content}</Paragraph>}
              />

              <p style={{ fontSize: 14, textAlign: "right" }}>
                {new Date(comment.time).toDateString()}
              </p>

              {comment.id === replying && (
                <ReplyBox
                  id={comment.id}
                  setComments={setComments}
                  comments={comments}
                />
              )}
              {comment.replies.length > 0 && (
                <ReplyList replies={comment.replies} />
              )}
            </Card>
          </Flex>
        </List.Item>
      )}
      pagination={{
        pageSize: 5,
        showSizeChanger: true,
        position: "bottom",
      }}
    />
  );
};

export default CommentList;
