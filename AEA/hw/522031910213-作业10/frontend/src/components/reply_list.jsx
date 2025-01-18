import { Avatar, Card, Collapse, Typography } from "antd";
import { useEffect, useState } from "react";
import { Link } from "react-router-dom";
import { getOtherUser } from "../services/userService";
const { Paragraph, Text } = Typography;
export const ReplyList = ({ replies }) => {
  const ReplyCard = (replies) => {
    const [users, setUsers] = useState({});
    useEffect(() => {
      // get other user info
      replies.map((reply) => {
        if (users[reply.uid] === undefined) {
          getOtherUser(reply.uid).then((res) => {
            setUsers((users) => ({ ...users, [reply.uid]: res }));
          });
        }
      });
    }, [replies]);
    return replies.map((reply, index) => (
      <Card size="small" bordered={false} key={index}>
        <Card.Meta
          avatar={
            <Link to={`/user/${reply.uid}`}>
              <Avatar src={users[reply.uid]?.avatar} />
            </Link>
          }
          title={users[reply.uid]?.username}
          description={<Paragraph>{reply.content}</Paragraph>}
        />
        <p style={{ fontSize: 12, textAlign: "right" }}>
          {new Date(reply.time).toDateString()}
        </p>
      </Card>
    ));
  };
  const Items = [
    {
      key: "1",
      label: `回复 (${replies.length})`,
      children: ReplyCard(replies),
    },
  ];
  return <Collapse ghost size="small" items={Items} />;
};
