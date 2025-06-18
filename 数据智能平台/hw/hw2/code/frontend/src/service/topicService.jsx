import { PREFIX, del, getJson, postFormData } from "./common";

//定义主题服务的url前缀
const TOPICPREFIX = `${PREFIX}/topic`;

//获取所有主题
export async function getAllTopics() {
  const url = `${TOPICPREFIX}/all_topics`;
  let result;

  try {
    result = await getJson(url);
  } catch (e) {
    console.log(e);
  }
  //   console.log(result);
  return result;
}

// 根据主题获得对应的设备
export async function getDevicesByTopic(topic) {
  const url = `${TOPICPREFIX}/get_tables_by_column?column_name=${topic}`;
  let result;

  try {
    result = await getJson(url);
  } catch (e) {
    console.log(e);
  }
  //   console.log(result);
  return result;
}
