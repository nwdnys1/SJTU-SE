import { BASEURL, get } from "./requestService";
const PREFIX = `${BASEURL}/api/tag`;

export async function getTags() {
  const url = `${PREFIX}/list`;
  let result;
  result = await get(url);
  return result;
}
export async function getRelatedTags(name) {
  const url = `${PREFIX}/related?name=${name}`;
  let result;
  result = await get(url);
  return result;
}