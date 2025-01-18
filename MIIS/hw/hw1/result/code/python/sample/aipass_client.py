import base64
import copy
import json

import requests
import jsonpath_rw

from sample import ne_utils
from data import response_path_list
from urllib import parse

media_type_list = ["text", "audio", "image", "video"]


# 准备请求数据
def prepare_req_data(request_data):
    new_request_data = copy.deepcopy(request_data)
    media_path2name = {}
    for media_type in media_type_list:
        media_expr = jsonpath_rw.parse("$..payload.*.{}".format(media_type))
        media_match = media_expr.find(new_request_data)
        if len(media_match) > 0:
            for media in media_match:
                media_path2name[str(media.full_path)] = media.value
    for media_path, media_name in media_path2name.items():
        payload_path_list = media_path.split(".")
        f_data = ne_utils.get_file_bytes(media_name)
        new_request_data["header"]["status"] = 3
        new_request_data["payload"][payload_path_list[1]][payload_path_list[2]] = (
            base64.b64encode(f_data).decode()
        )
        new_request_data["payload"][payload_path_list[1]]["status"] = 3
    return new_request_data


# 执行http请求
def execute(request_url, request_data, method, app_id, api_key, api_secret, response_json):

    # 获取请求url
    auth_request_url = ne_utils.build_auth_request_url(
        request_url, method, api_key, api_secret
    )

    url_result = parse.urlparse(request_url)
    headers = {
        "content-type": "application/json",
        "host": url_result.hostname,
        "app_id": app_id,
    }
    # 准备待发送的数据
    new_request_data = prepare_req_data(request_data)
    # print("请求数据:{}\n".format(new_request_data))
    response = requests.post(
        auth_request_url, data=json.dumps(new_request_data), headers=headers
    )
    return deal_response(response,response_json)


# 处理响应数据
def deal_response(response,response_json):
    temp_result = json.loads(response.content.decode())
    print("响应数据:{}\n".format(temp_result))
    #response_json.append(temp_result)
    header = temp_result.get("header")
    if header is None:
        return
    code = header.get("code")
    if header is None or code != 0:
        print("获取结果失败，请根据code查证问题原因")
        
    print("sid:{}".format(header.get("sid")))
    return temp_result

    # 打印Base64解码后数据并生成文件
    # if response_path_list is None or len(response_path_list) == 0:
    #     return
    # for response_path in response_path_list:
    #     response_expr = jsonpath_rw.parse(response_path)
    #     response_match = response_expr.find(temp_result)
    #     if len(response_match) > 0:
    #         for response_item in response_match:
    #             if response_item.value is None:
    #                 continue
    #             encoding = response_item.value.get("encoding")
    #             if encoding is None or len(encoding) == 0:
    #                 continue
    #             for media_type in media_type_list:
    #                 media_value = response_item.value.get(media_type)
    #                 if media_value is None or len(media_value) == 0:
    #                     continue
    #                 real_data = base64.b64decode(media_value)
    #                 response_path_split_list = response_path.split(".")
    #                 write_file_path = "./resource/output/{}.{}".format(
    #                     response_path_split_list[len(response_path_split_list) - 1],
    #                     encoding,
    #                 )
    #                 with open(write_file_path, "ab") as file:
    #                     file.write(real_data)
