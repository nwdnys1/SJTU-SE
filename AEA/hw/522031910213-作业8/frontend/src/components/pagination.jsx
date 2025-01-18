export const ListPagination = () => {
  return {
    defaultPageSize: 3,
    showQuickJumper: true,
    showSizeChanger: true,
    pageSizeOptions: ["1", "3", "5", "10", "20", "50"],
    showTotal: (total, range) => `${total} 项中的 ${range[0]}-${range[1]} 项 `,
    position: "bottom",
    align: "center",
  };
};

export const BlockPagination = () => {
  return {
    pageSize: 5,
    showSizeChanger: true,
    position: "top",
  };
};
