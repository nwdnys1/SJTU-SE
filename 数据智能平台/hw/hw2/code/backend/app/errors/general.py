from flask import jsonify
from app.utils.response import ApiResponse

def register_general_error_handlers(app):
    """注册通用错误处理器"""
    @app.errorhandler(404)
    def not_found_error(error):
        return ApiResponse(data=None, code=404, message="Not Found").to_json()

    @app.errorhandler(500)
    def internal_error(error):
        return ApiResponse(data=None, code=500, message="Internal Server Error").to_json()

    # 捕获所有 HTTP 异常
    @app.errorhandler(400)
    @app.errorhandler(401)
    @app.errorhandler(403)
    def http_error(error):
        return ApiResponse(data=None, code=error.code, message=error.description).to_json()

    # 捕获所有未处理的异常
    @app.errorhandler(Exception)
    def handle_unexpected_exception(error):
        # 记录日志（可选）
        app.logger.error(f"Unexpected error: {str(error)}")
        return ApiResponse(data=None, code=500, message="Internal Server Error").to_json()