import { BASEURL, Delete, get, post, put } from "./requestService";
const PREFIX = `${BASEURL}/api/cart`;

export const getCartItems = async () => {
  const url = `${PREFIX}/list`;
  let result;
  result = await get(url);
  return result;
};

export const deleteCartItem = async (id) => {
  const url = `${PREFIX}/delete/${id}`;
  let result;
  try {
    result = await Delete(url);
    return result;
  } catch (e) {
    console.log(e);
    alert(e);
  }
};

export const addCartItem = async (bookId) => {
  const url = `${PREFIX}/add/${bookId}`;
  let result;
  try {
    result = await post(url);
    alert("已加入购物车，请前往购物车结算");
    return result;
  } catch (e) {
    console.log(e);
    alert(e);
  }
};

export const updateCartItemNumber = async (id, number) => {
  const url = `${PREFIX}/update/${id}?quantity=${number}`;
  let result;
  try {
    result = await put(url);
    return result;
  } catch (e) {
    console.log(e);
    alert(e);
  }
};
