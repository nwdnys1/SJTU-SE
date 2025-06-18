import { PREFIX, del, getJson, postFormData } from "./common";

//定义系统服务的url前缀
const SYSTEMPREFIX = `${PREFIX}/system`;

//获取实时数据
export async function getRealTimeData() {
  const url = `${SYSTEMPREFIX}/realtime_data`;
  let result;

  try {
    result = await getJson(url);
  } catch (e) {
    console.log(e);
  }
  //   console.log(result);
  return result;
}
