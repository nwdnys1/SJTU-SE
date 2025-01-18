import { BASEURL, get, post } from "./requestService";
const PREFIX = `${BASEURL}/api/order`;
export const getOrders = async () => {
  const url = `${PREFIX}/list`;
  let result;
  result = await get(url);
  return result;
};
export const searchAllOrders = async ({keyword,page,pageSize,start,end }) => {
  const url = `${PREFIX}/admin/search?keyword=${keyword}&page=${page}&pageSize=${pageSize}&start=${start}&end=${end}`;
  let result;
  result = await get(url);
  return result;
}

export const searchOrders = async ({ keyword,start, end,page,pageSize }) => {
  const url = `${PREFIX}/search?keyword=${keyword}&start=${start}&end=${end}&page=${page}&pageSize=${pageSize}`;
  let result;
  result = await get(url);
  return result;
};



export const placeOrder = async (orderRequest) => {
  const url = `${PREFIX}/add`;
  let result;
  try {
    result = await post(url, orderRequest);
    return result;
  } catch (e) {
    console.log(e);
    alert(e);
  }
};
