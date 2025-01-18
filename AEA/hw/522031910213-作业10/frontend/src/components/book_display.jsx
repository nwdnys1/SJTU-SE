import { Button, Flex, Row } from "antd";
import SwitchButton from "./switchbtn";
import { ListLayout } from "./book_list";
import { BlockLayout } from "./book_block";
import { Link, useSearchParams } from "react-router-dom";
import { defaultLayout } from "../utils/config";

const BookDisplay = ({ books, withButton, length }) => {
  //switchbtn的回调是修改layout 所以handleChange函数值修改layout 并根据layout设置pageSize page设为1
  const [searchParams, setSearchParams] = useSearchParams();
  const layout = searchParams.get("layout") || defaultLayout; //默认为block
  const handleChange = (layout) => {
    setSearchParams({
      page: 1,
      pageSize: layout === "block" ? 12 : 4,
      keyword: searchParams.get("keyword") || "",
      tag: searchParams.getAll("tag") || [],
      layout: layout,
    });
  };
  return (
    <Flex vertical gap={20} style={{ width: "100%" }}>
      <Row justify={"space-between"}>
        <SwitchButton handleChange={handleChange} layout={layout} />
        {withButton && (
          <Link to="/allbooks/list" style={{ fontSize: "20px" }}>
            <Button type="primary" size="large">
              浏览全部！
            </Button>
          </Link>
        )}
      </Row>
      {layout === "list" ? (
        <ListLayout books={books} length={length} paging={!withButton} />
      ) : (
        <BlockLayout
          books={books}
          length={length}
          fixedGrid={withButton}
          paging={!withButton}
        />
      )}
    </Flex>
  );
};

export default BookDisplay;
