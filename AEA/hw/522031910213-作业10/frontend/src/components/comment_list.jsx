import React, { useEffect, useState } from "react";
import { Card, Avatar, List, Flex, Typography, Row, Space } from "antd";
import ReplyBox from "./reply_box";
import { Link } from "react-router-dom";
import { ReplyList } from "./reply_list";
import {
  CloseCircleOutlined,
  LikeOutlined,
  LikeTwoTone,
  MessageOutlined,
} from "@ant-design/icons";
import { useAuth } from "../context/authContext";
import { likeComment } from "../services/commentService";
import { getOtherUser } from "../services/userService";
import { set } from "@ant-design/plots/es/core/utils";
const { Paragraph } = Typography;
const CommentList = ({ comments, setComments }) => {
  const { user, setUser } = useAuth();
  const [replying, setReplying] = useState(null);
  const [users, setUsers] = useState(user ? { [user.id]: user } : {});

  useEffect(() => {
    // get other user info
    comments.map((comment) => {
      if (users[comment.uid] === undefined) {
        getOtherUser(comment.uid).then((res) => {
          setUsers((users) => ({ ...users, [comment.uid]: res }));
        });
      }
    });
  }, [comments]);

  const handleReply = (id) => {
    setReplying(replying === id ? null : id);
  };

  const handleLike = (cid) => {
    likeComment(cid)
      .then((res) => {
        if (res == "取消点赞") {
          setComments(
            comments.map((comment) => {
              if (comment.id === cid) {
                comment.likes = comment.likes.filter((id) => id !== user.id);
              }
              return comment;
            })
          );
        } else {
          setComments(
            comments.map((comment) => {
              if (comment.id === cid) {
                comment.likes.push(user.id);
              }
              return comment;
            })
          );
        }
      })
      .catch((e) => {
        alert(e);
      });
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
                <Space size={3} onClick={() => handleLike(comment.id)}>
                  {comment.likes.includes(user?.id) ? (
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
                  <Link to={`/user/${comment.uid}`}>
                    <Avatar src={users[comment.uid]?.avatar} size="large" />
                  </Link>
                }
                title={users[comment.uid]?.username}
                description={<Paragraph>{comment.content}</Paragraph>}
              />
              <p style={{ fontSize: 14, textAlign: "right" }}>
                {new Date(comment.time).toDateString()}
              </p>
              {comment.replies.length > 0 && (
                <ReplyList replies={comment.replies} />
              )}{" "}
              {comment.id === replying && (
                <ReplyBox id={comment.id} setComments={setComments} />
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
