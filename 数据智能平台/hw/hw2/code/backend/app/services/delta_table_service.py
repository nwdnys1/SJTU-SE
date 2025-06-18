from app.services.spark_service import SparkService
from pyspark.sql import DataFrame

class DeltaTableService:
    @staticmethod
    def list_delta_tables_info(hdfs_path):
        spark = SparkService.get_spark_session()
        hadoop_conf = spark.sparkContext._jsc.hadoopConfiguration()
        path = spark._jvm.org.apache.hadoop.fs.Path(hdfs_path)
        fs = path.getFileSystem(hadoop_conf)

        statuses = fs.listStatus(path)
        delta_tables_info = []
        for status in statuses:
            if status.isDirectory():
                folder_path = status.getPath().toString()
                if fs.exists(spark._jvm.org.apache.hadoop.fs.Path(folder_path + "/_delta_log")):
                    table_name = status.getPath().getName()
                    delta_tables_info.append({
                        "table_name": table_name,
                        "table_path": folder_path
                    })

        return delta_tables_info

    @staticmethod
    def get_table_columns(table_path: str) -> list:
        """
        获取指定 Delta 表的所有列名。

        :param table_path: Delta 表的路径
        :return: 列名的列表
        """
        spark = SparkService.get_spark_session()
        # 读取 Delta 表的元数据
        df = spark.read.format("delta").load(table_path)
        # 获取列名
        columns = df.columns
        return columns

    @staticmethod
    def get_all_delta_tables_columns(hdfs_path: str) -> dict:
        """
        获取指定 HDFS 路径下所有 Delta 表的列名。

        :param hdfs_path: HDFS 路径
        :return: 包含列名的字典
        """
        delta_tables_info = DeltaTableService.list_delta_tables_info(hdfs_path)
        all_tables_columns = {}
        for table_info in delta_tables_info:
            table_name = table_info["table_name"]
            table_path = table_info["table_path"]
            columns = DeltaTableService.get_table_columns(table_path)
            all_tables_columns[table_name] = columns

        # 然后去重、并去掉timestamp列名
        all_columns = set()
        for columns in all_tables_columns.values():
            all_columns.update(columns)
        all_columns.discard("timestamp")
        # 将集合转换为列表
        all_columns = list(all_columns)

        return all_columns
    
    @staticmethod
    def get_tables_by_column(hdfs_path: str, column_name: str) -> list:
        """
        获取指定 HDFS 路径下所有包含指定列名的 Delta 表名。

        :param hdfs_path: HDFS 路径
        :param column_name: 列名
        :return: 包含指定列名的 Delta 表名列表
        """
        delta_tables_info = DeltaTableService.list_delta_tables_info(hdfs_path)
        matching_tables = []
        for table_info in delta_tables_info:
            table_name = table_info["table_name"]
            table_path = table_info["table_path"]
            columns = DeltaTableService.get_table_columns(table_path)
            if column_name in columns:
                matching_tables.append(table_name)

        return matching_tables