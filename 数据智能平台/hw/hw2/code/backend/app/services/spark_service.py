from pyspark.sql import SparkSession
from app.config import Config

class SparkService:
    _spark = None

    @classmethod
    def get_spark_session(cls):
        if cls._spark is None:
            cls._spark = SparkSession.builder \
                .appName("FlaskDeltaLakeApp") \
                .config("spark.master", Config.SPARK_MASTER) \
                .config("spark.submit.deployMode", Config.SPARK_DEPLOY_MODE) \
                .config("spark.sql.extensions", Config.SPARK_SQL_EXTENSIONS) \
                .config("spark.sql.catalog.spark_catalog", Config.SPARK_SQL_CATALOG) \
                .config("spark.jars.packages", Config.SPARK_JARS_PACKAGES) \
                .config("spark.jars.repositories", Config.SPARK_JARS_REPOSITORIES) \
                .getOrCreate()
        return cls._spark