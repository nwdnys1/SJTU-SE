class Config:
    SPARK_MASTER = "local"
    SPARK_DEPLOY_MODE = "client"
    SPARK_SQL_EXTENSIONS = "io.delta.sql.DeltaSparkSessionExtension"
    SPARK_SQL_CATALOG = "org.apache.spark.sql.delta.catalog.DeltaCatalog"
    SPARK_JARS_PACKAGES = "io.delta:delta-core_2.13:2.4.0"
    SPARK_JARS_REPOSITORIES = "https://repo1.maven.org/maven2"
    BASE_PATH = "/tmp/delta_tables"