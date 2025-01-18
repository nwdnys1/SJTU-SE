// 首页热门话题的标签栏
import { Tag, Row } from "antd";
import { Link } from "react-router-dom";

const TagBar = ({ tags }) => {
  return (
    tags && (
      <Row align={"middle"}>
        {tags.map((tag) => (
          <Link to={`/allbooks/category?tag=${tag.id}`} key={tag.id}>
            <Tag key={tag.id} color="blue">
              {tag.content}
            </Tag>
          </Link>
        ))}
      </Row>
    )
  );
};
export default TagBar;
