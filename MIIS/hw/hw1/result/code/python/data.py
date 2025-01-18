class APP:
    def __init__(self, APPId: str, APIKey: str, APISecret: str):
        self.APPId = APPId
        self.APIKey = APIKey
        self.APISecret = APISecret


# 创建 APP 实例并存储在列表中
APP_list = [
    APP(
        APPId="",
        APIKey="",
        APISecret="",
    ),
]


# 请求数据
def request_data(image_path, APPId):
    return {
        "header": {"app_id": APPId, "status": 3},
        "parameter": {
            "image_recognize": {
                "output_text_result": {
                    "encoding": "utf8",
                    "compress": "raw",
                    "format": "plain",
                }
            }
        },
        "payload": {"image": {"encoding": "png", "image": image_path, "status": 3}},
    }


# 请求地址
request_url = "https://api.xf-yun.com/v1/private/sc45f0684"

# 用于快速定位响应值

response_path_list = [
    "$..payload.output_text_result",
]
