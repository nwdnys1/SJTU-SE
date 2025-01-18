import { Avatar, Card, Collapse, Typography } from "antd";
import { Link } from "react-router-dom";
const { Paragraph, Text } = Typography;
export const ReplyList = ({ replies }) => {
  const ReplyCard = (replies) => {
    return replies.map((reply, index) => (
      <Card size="small" bordered={false} key={index}>
        <Card.Meta
          avatar={
            <Link to={`/user/${reply.user.username}`}>
              <Avatar src={reply.user.avatar} />
            </Link>
          }
          title={reply.user.username}
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
