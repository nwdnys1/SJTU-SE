from flask_cors import CORS

def configure_cors(app):
    """
    配置 CORS
    :param app: Flask 应用实例
    """
    CORS(
        app,
        supports_credentials=True,
        resources={
            r"/*": {
                "origins": [
                    "http://localhost:5173",
                    "https://10.80.173.84:5173"
                ]
            }
        }
    )