from flask import jsonify

class ApiResponse:
    def __init__(self, data=None, code=200, message="Success"):
        """
        自定义 API 响应类
        :param data: 返回的数据，默认为空字典
        :param code: HTTP 状态码，默认为 200
        :param message: 返回的消息，默认为 "Success"
        """
        self.data = data
        self.code = code
        self.message = message

    def to_json(self):
        """
        将响应对象转换为 JSON 格式
        """
        return jsonify({
            "code": self.code,
            "message": self.message,
            "data": self.data
        }), self.code