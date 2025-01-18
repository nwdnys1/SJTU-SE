import { BASEURL, get, post, put } from "./requestService";
const PREFIX = `${BASEURL}/api/user`;

export async function getUser() {
  const url = `${PREFIX}/get`;
  let result;
  result = await get(url);
  return result;
}

export async function getAllUsers() {
  const url = `${PREFIX}/admin/list`;
  let result;
  result = await get(url);
  return result;
}

export async function updateUser(user) {
  const url = `${PREFIX}/update`;
  let result;
  result = await put(url, user);
  return result;
}

export async function getOtherUser(uid) {
  const url = `${PREFIX}/get/${uid}`;
  let result;
  result = await get(url);
  return result;
}

export async function disableUser(id) {
  const url = `${PREFIX}/admin/disable/${id}`;
  let result;
  result = await put(url);
  return result;
}

export async function enableUser(id) {
  const url = `${PREFIX}/admin/enable/${id}`;
  let result;
  result = await put(url);
  return result;
}

export async function getTopUsers({ start, end }) {
  const url = `${PREFIX}/admin/rank?start=${start}&end=${end}&nums=10`;
  let result;
  result = await get(url);
  return result;
}

export async function getPurchasedBooks({ start, end }) {
  const url = `${PREFIX}/statistics?start=${start}&end=${end}`;
  let result;

  result = await get(url);
  return result;
}
