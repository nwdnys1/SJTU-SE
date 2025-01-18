#!/usr/bin/env python3
# -*-coding:utf-8 -*-
from sample.aipass_client import execute
from data import *
import os
import json

if __name__ == "__main__":

    response_list = []
    (
        os.remove("../../flows/work/.jar/data.json")
        if os.path.exists("../../flows/work/.jar/data.json")
        else None
    )

    def ocr(path, data_set, cnt=0):
        for dirpath, dirnames, filenames in os.walk(path):
            i = 0
            while i < len(filenames):
                filename = filenames[i]
                file_path = os.path.join(dirpath, filename)
                response = execute(
                    request_url,
                    request_data(file_path, APP_list[cnt % len(APP_list)].APPId),
                    "POST",
                    APP_list[cnt % len(APP_list)].APPId,
                    APP_list[cnt % len(APP_list)].APIKey,
                    APP_list[cnt % len(APP_list)].APISecret,
                    response_list,
                )
                print(response["header"]["code"])
                # 11201表示请求次数超限，需要更换APP
                if response["header"]["code"] == 11201:
                    cnt += 1
                    continue
                response["file_name"] = filename
                response["data_set"] = data_set  # 用于区分数据集
                response_list.append(response)
                cnt += 1  # 用于循环使用APP_list中的不同KEY
                i += 1

    ocr("../../flows/work/input/1", 1)
    ocr("../../flows/work/input/2", 2)
    #ocr("../../flows/work/input/test", 2)

    with open("../../flows/work/.jar/data.json", "a") as json_file:
        json.dump(response_list, json_file, ensure_ascii=False)
