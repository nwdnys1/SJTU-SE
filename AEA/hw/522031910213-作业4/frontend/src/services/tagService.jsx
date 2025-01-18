import { BASEURL, get } from "./requestService";
const PREFIX = `${BASEURL}/api/tag`;

export async function getTags() {
  const url = `${PREFIX}/list`;
  let result;
  try {
    result = await get(url);
    return result;
  } catch (e) {
    console.log(e);
  }
}
