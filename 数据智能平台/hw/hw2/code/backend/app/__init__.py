from flask import Flask
from .config import Config
from .routes import device
from .routes import system
from .routes import predict
from .routes import topic
from .routes import html
from .services.spark_service import SparkService
from .errors.general import register_general_error_handlers
from .cors import configure_cors

def create_app():
    app = Flask(__name__)
    app.config.from_object(Config)

    # 初始化数据库连接
    SparkService.get_spark_session()
    print("SparkSession initialized.")

    # 注册错误处理器
    register_general_error_handlers(app)

    # 配置 CORS
    configure_cors(app)

    # 注册蓝图
    app.register_blueprint(device.bp, url_prefix='/api/devices')
    app.register_blueprint(system.bp, url_prefix='/api/system')
    app.register_blueprint(predict.bp, url_prefix='/api/predict')
    app.register_blueprint(topic.bp, url_prefix='/api/topic')
    app.register_blueprint(html.bp, url_prefix='/api/html')

    return app