import { BASEURL, get, post, postText } from "./requestService";
const PREFIX = `${BASEURL}/api/like`;

export const likeComment = async (cid) => {
  const url = `${PREFIX}/${cid}`;
  let result;

  result = await post(url);
  return result;
};

export const getCids = async () => {
  const url = `${PREFIX}/list`;
  let result;

  result = await get(url);
  return result;
};
