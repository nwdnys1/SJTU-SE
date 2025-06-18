import pandas as pd
import re
from pathlib import Path
import numpy as np
import gc
from datetime import datetime
import psutil
from concurrent.futures import ThreadPoolExecutor, as_completed
import threading
import queue


class ThreadedCSVProcessor:
    def __init__(self):
        # 定义需要保留的列
        self.selected_columns = [
            "flightDate",
            "startingAirport",
            "destinationAirport",
            "isNonStop",
            "baseFare",
            "totalFare",
            "travelDuration",
            "totalTravelDistance",
        ]

        # 优化数据类型配置
        self.dtype = {
            "baseFare": "float32",
            "totalFare": "float32",
            "totalTravelDistance": "float32",
            "isNonStop": "bool",
        }

        # 线程安全的数据结构
        self.log_lock = threading.Lock()
        self.chunk_queue = queue.Queue(maxsize=10)  # 控制内存使用的队列
        self.processed_count = 0
        self.total_rows = 0

    def _log(self, message):
        """线程安全的日志记录"""
        timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        log_message = f"[{timestamp}] {message}"

        with self.log_lock:
            print(log_message)
            with open(self.log_file, "a") as f:
                f.write(log_message + "\n")

    def _parse_duration(self, duration_str):
        """安全解析持续时间"""
        try:
            hours = minutes = 0
            if pd.isna(duration_str):
                return np.nan

            hour_match = re.search(r"(\d+)H", duration_str)
            minute_match = re.search(r"(\d+)M", duration_str)

            if hour_match:
                hours = float(hour_match.group(1))
            if minute_match:
                minutes = float(minute_match.group(1))

            return hours + minutes / 60
        except Exception as e:
            self._log(f"Duration解析错误: {str(e)}")
            return np.nan

    def _process_chunk(self, chunk, chunk_id):
        """处理单个数据块（线程安全）"""
        try:
            # 1. 列选择
            chunk = chunk.loc[:, self.selected_columns]

            # 2. 类型转换
            chunk["flightDate"] = pd.to_datetime(chunk["flightDate"], errors="coerce")
            chunk["travelDuration"] = chunk["travelDuration"].apply(
                self._parse_duration
            )
            chunk["isNonStop"] = chunk["isNonStop"].astype(bool)

            # 3. 过滤无效数据
            chunk = chunk[
                (chunk["baseFare"] > 0)
                & (chunk["totalFare"] > 0)
                & (chunk["totalTravelDistance"] > 0)
                & (chunk["travelDuration"].between(0.1, 48, inclusive="both"))
                & (chunk["flightDate"].notna())
            ].copy()

            # 4. 添加衍生特征
            chunk["year"] = chunk["flightDate"].dt.year.astype("int16")
            chunk["month"] = chunk["flightDate"].dt.month.astype("int8")
            chunk["day_of_week"] = chunk["flightDate"].dt.dayofweek.astype("int8")
            chunk["fare_per_mile"] = (
                chunk["totalFare"] / chunk["totalTravelDistance"]
            ).astype("float32")

            # 保存处理结果
            output_path = self.output_dir / f"cleaned_chunk_{chunk_id:04d}.csv"
            chunk.to_csv(output_path, index=False)

            # 更新统计信息
            with self.log_lock:
                self.processed_count += len(chunk)
                self.total_rows += len(chunk)

            return len(chunk)

        except Exception as e:
            self._log(f"处理分块 {chunk_id} 时出错: {str(e)}")
            return 0
        finally:
            # 确保释放内存
            del chunk
            gc.collect()

    def _reader_thread(self, input_path, chunksize):
        """读取线程，将数据块放入队列"""
        try:
            reader = pd.read_csv(
                input_path,
                chunksize=chunksize,
                dtype=self.dtype,
                usecols=self.selected_columns,
                memory_map=True,
                low_memory=False,
            )

            for chunk_id, chunk in enumerate(reader):
                self.chunk_queue.put((chunk_id, chunk))
                self._log(f"已加载分块 {chunk_id} 到队列")

            # 放入结束标记
            self.chunk_queue.put((None, None))
            self._log("所有分块已加载完成")

        except Exception as e:
            self._log(f"读取线程出错: {str(e)}")
            self.chunk_queue.put((None, None))

    def process_large_csv(
        self, input_path, output_dir, chunksize=500000, max_workers=4
    ):
        """
        多线程处理大CSV文件
        :param input_path: 输入文件路径
        :param output_dir: 输出目录
        :param chunksize: 每个分块的行数
        :param max_workers: 最大线程数
        """
        # 准备输出目录
        self.output_dir = Path(output_dir)
        self.output_dir.mkdir(exist_ok=True)
        self.log_file = self.output_dir / "processing_log.txt"

        # 初始化日志
        self._log(f"开始处理: {input_path}")
        self._log(f"分块大小: {chunksize} 行")
        self._log(f"线程数: {max_workers}")
        self._log(f"当前内存: {psutil.Process().memory_info().rss / 1024 ** 2:.2f} MB")

        # 启动读取线程
        reader_thread = threading.Thread(
            target=self._reader_thread, args=(input_path, chunksize), daemon=True
        )
        reader_thread.start()

        # 使用线程池处理数据
        start_time = datetime.now()
        with ThreadPoolExecutor(max_workers=max_workers) as executor:
            futures = []

            while True:
                # 从队列获取数据块
                chunk_id, chunk = self.chunk_queue.get()

                # 结束标记
                if chunk_id is None:
                    break

                # 提交处理任务
                future = executor.submit(self._process_chunk, chunk, chunk_id)
                futures.append(future)
                self._log(f"已提交分块 {chunk_id} 到线程池")

                # 手动释放引用
                del chunk

            # 等待所有任务完成
            for future in as_completed(futures):
                try:
                    result = future.result()
                    self._log(f"处理完成: {result} 行")
                except Exception as e:
                    self._log(f"处理失败: {str(e)}")

        # 性能统计
        time_elapsed = (datetime.now() - start_time).total_seconds()
        self._log("\n处理摘要:")
        self._log(f"总处理行数: {self.total_rows}")
        self._log(f"总耗时: {time_elapsed:.2f} 秒")
        self._log(f"处理速度: {self.total_rows/time_elapsed:.2f} 行/秒")
        self._log(f"峰值内存: {psutil.Process().memory_info().rss / 1024 ** 2:.2f} MB")
        self._log(f"输出目录: {output_dir}")


if __name__ == "__main__":
    # 配置参数
    config = {
        "input_path": "data/itineraries.csv",  # 输入文件路径
        "output_dir": "flight_prices",  # 输出目录
        "chunksize": 10000000,  # 每个分块的行数
        "max_workers": 8,  # 线程数 (建议为CPU核心数的1-2倍)
    }

    # 执行处理
    processor = ThreadedCSVProcessor()
    processor.process_large_csv(**config)
