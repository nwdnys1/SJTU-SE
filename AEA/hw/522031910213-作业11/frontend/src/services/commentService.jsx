import { BASEURL, get, post, postText } from "./requestService";
const PREFIX = `${BASEURL}/api/comment`;

export const getCommentsByBookId = async (bookId) => {
  const url = `${PREFIX}/list/${bookId}`;
  let result;
  try {
    result = await get(url);
    return result;
  } catch (e) {
    console.log(e);
    alert(e);
  }
};
export const addComment = async (comment) => {
  const url = `${PREFIX}/add`;
  let result;

  result = await post(url, comment);
  return result;
};
export const deleteComment = async (id) => {
  const url = `${PREFIX}/delete/${id}`;
  let result;
  try {
    result = await post(url);
    return result;
  } catch (e) {
    console.log(e);
    alert(e);
  }
};
export const addReply = async ({ cid, content }) => {
  const url = `${PREFIX}/reply/${cid}`;
  let result;

  result = await postText(url, content);
  return result;
};

export const likeComment = async (cid) => {
  const url = `${PREFIX}/like/${cid}`;
  let result;

  result = await post(url);
  return result;
};
