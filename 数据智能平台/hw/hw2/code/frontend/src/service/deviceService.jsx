import { PREFIX, del, getJson, postFormData } from "./common";

//定义设备服务的url前缀
const DEVICEPREFIX = `${PREFIX}/devices`;

//获取所有标签
export async function getAllDevices() {
  const url = `${DEVICEPREFIX}/all_device_names`;
  let result;

  try {
    result = await getJson(url);
  } catch (e) {
    console.log(e);
  }
  //   console.log(result);
  return result;
}

//获取指定设备的所有数据
export async function getDeviceData(deviceName, month) {
  const url = `${DEVICEPREFIX}/device_data?device_name=${deviceName}&month=${month}`;
  let result;

  try {
    result = await getJson(url);
  } catch (e) {
    console.log(e);
  }
  //   console.log(result);
  return result;
}
