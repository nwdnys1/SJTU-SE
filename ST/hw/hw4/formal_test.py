import openai
import time
import threading
from typing import List, Dict
import statistics
import matplotlib
matplotlib.use('Agg')  # 在导入pyplot之前设置
import matplotlib.pyplot as plt
import json
import os
from datetime import datetime
import random
import re
import numpy as np
import pynvml  # 用于GPU监控
from concurrent.futures import ThreadPoolExecutor

# 初始化NVML
try:
    pynvml.nvmlInit()
    gpu_available = True
except:
    gpu_available = False

class GPUMonitor:
    """GPU监控类"""
    def __init__(self):
        self.gpu_utilizations = []
        self.gpu_memories = []
        self.timestamps = []
        self.running = False
        
    def start_monitoring(self, interval=0.5):
        """启动监控线程"""
        self.running = True
        monitor_thread = threading.Thread(target=self._monitor_loop, args=(interval,))
        monitor_thread.daemon = True
        monitor_thread.start()
        
    def _monitor_loop(self, interval):
        """监控循环"""
        while self.running:
            try:
                handle = pynvml.nvmlDeviceGetHandleByIndex(0)
                util = pynvml.nvmlDeviceGetUtilizationRates(handle).gpu
                mem = pynvml.nvmlDeviceGetMemoryInfo(handle).used / (1024 ** 2)  # MB
                
                self.gpu_utilizations.append(util)
                self.gpu_memories.append(mem)
                self.timestamps.append(time.time())
            except Exception as e:
                print(f"GPU监控错误: {e}")
            time.sleep(interval)
            
    def stop_monitoring(self):
        """停止监控"""
        self.running = False

# Configure OpenAI client for Ollama
client = openai.OpenAI(
    base_url="http://localhost:11434/v1",
    api_key="no-key-required"
)

MODEL_NAME = "Qwen/Qwen3-1.7B"
OUTPUT_DIR = "results/" + "Qwen3-1.7B" + "/" + "Vllm_test_results"
os.makedirs(OUTPUT_DIR, exist_ok=True)

class PerformanceMetrics:
    def __init__(self, scenario_name: str):
        self.scenario_name = scenario_name
        self.ttft_list = []    # Time-To-First-Token
        self.tbt_list = []     # Time-Between-Tokens
        self.tps_list = []     # Tokens-Per-Second
        self.total_tokens = 0
        self.concurrency = 0
        self.prompts_used = []
        self.lock = threading.Lock()
        self.gpu_monitor = GPUMonitor() if gpu_available else None
        
    def add_percentiles(self, data):
        """计算百分位数"""
        if not data:
            return {}
        arr = np.array(data)
        return {
            'p50': np.percentile(arr, 50),
            'p90': np.percentile(arr, 90),
            'p95': np.percentile(arr, 95),
            'p99': np.percentile(arr, 99),
            'max': np.max(arr),
            'min': np.min(arr),
            'mean': np.mean(arr)
        }

def generate_dynamic_prompt(scenario_type: str) -> str:
    """Generate test prompts dynamically based on scenario type"""
    prompt_map = {
        "short_text": "Generate a very short (5-10 tokens) question about {}/no_think",
        "qa_medium": "Create a medium-length (50-100 tokens) question about {}/no_think",
        "long_form": "Make a detailed question with the length of 40-50 tokens about {} and this prompt ask llm to give a long answer/no_think",
        "stress_test": "Create a (30-50 tokens) question about {}/no_think",
        "mixed_load": "Provide a question about {} with variable length(10-200 tokens)/no_think"
    }
    
    topics = ["artificial intelligence", "quantum computing", 
             "climate change", "machine learning", "neuroscience"]
    
    topic = random.choice(topics)
    
    response = client.chat.completions.create(
        model=MODEL_NAME,
        messages=[{
            "role": "system",
            "content": "You are a prompt engineering assistant. Generate the requested test questions."
        }, {
            "role": "user",
            "content": prompt_map[scenario_type].format(topic)
        }],
        max_tokens=500,
        temperature=0.7
    )
    
    # Remove all content between <think> and </think> tags
    generated_prompt = re.sub(r'<think>.*?</think>', '', response.choices[0].message.content, flags=re.DOTALL)
    
    # Clean up any resulting extra whitespace
    generated_prompt = ' '.join(generated_prompt.split()).strip()
    
    return generated_prompt

def process_stream(stream, start_time: float, metrics: PerformanceMetrics):
    """Process synchronous stream response"""
    first_token_time = None
    token_count = 0
    tbt_list = []
    last_time = start_time
    
    for chunk in stream:
        current_time = time.time()
        if first_token_time is None:
            first_token_time = current_time - start_time
        else:
            tbt_list.append(current_time - last_time)
        token_count += 1
        last_time = current_time
    
    # Thread-safe metrics update
    with metrics.lock:
        if first_token_time is not None:
            metrics.ttft_list.append(first_token_time)
            metrics.tbt_list.extend(tbt_list)
            metrics.total_tokens += token_count
            duration = current_time - start_time
            if duration > 0:
                metrics.tps_list.append(token_count / duration)

def make_request(prompt: str, max_tokens: int, metrics: PerformanceMetrics):
    """Make a single API request and process the response"""
    start_time = time.time()
    
    try:
        # Create the stream (synchronous API)
        stream = client.chat.completions.create(
            model=MODEL_NAME,
            messages=[{"role": "user", "content": prompt + "/no_think"}],
            max_tokens=max_tokens,
            stream=True
        )
        
        # Process the stream
        process_stream(stream, start_time, metrics)
                
    except Exception as e:
        print(f"Error processing request: {str(e)}")

def run_scenario(scenario_type: str, concurrency: int, max_tokens: int, num_requests: int) -> PerformanceMetrics:
    """Run a test scenario with threading"""
    metrics = PerformanceMetrics(scenario_type)
    metrics.concurrency = concurrency
    
    # Generate unique prompts for this test run
    metrics.prompts_used = [generate_dynamic_prompt(scenario_type) for _ in range(num_requests)]

    time.sleep(5)  # Allow some time for the model to reset

    # 启动GPU监控
    if metrics.gpu_monitor:
        metrics.gpu_monitor.start_monitoring()
    
    # 使用ThreadPoolExecutor替代手动线程管理
    with ThreadPoolExecutor(max_workers=concurrency) as executor:
        futures = []
        for prompt in metrics.prompts_used:
            future = executor.submit(make_request, prompt, max_tokens, metrics)
            futures.append(future)
        
        # 等待所有任务完成
        for future in futures:
            future.result()
    
    # 停止GPU监控
    if metrics.gpu_monitor:
        metrics.gpu_monitor.stop_monitoring()
    
    return metrics

def save_results(metrics: PerformanceMetrics):
    """Save test results and generate plots"""
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    base_filename = f"{OUTPUT_DIR}/{metrics.scenario_name}_{timestamp}"
    
    # 计算百分位数
    ttft_percentiles = metrics.add_percentiles(metrics.ttft_list)
    tbt_percentiles = metrics.add_percentiles(metrics.tbt_list)
    tps_percentiles = metrics.add_percentiles(metrics.tps_list)
    
    # Save raw data
    with open(f"{base_filename}.json", "w") as f:
        json.dump({
            "model": MODEL_NAME,
            "concurrency": metrics.concurrency,
            "ttft": {
                "values": metrics.ttft_list,
                "percentiles": ttft_percentiles
            },
            "tbt": {
                "values": metrics.tbt_list,
                "percentiles": tbt_percentiles
            },
            "tps": {
                "values": metrics.tps_list,
                "percentiles": tps_percentiles
            },
            "total_tokens": metrics.total_tokens,
            "prompts": metrics.prompts_used,
            "gpu_metrics": {
                "utilization": metrics.gpu_monitor.gpu_utilizations if metrics.gpu_monitor else [],
                "memory": metrics.gpu_monitor.gpu_memories if metrics.gpu_monitor else [],
                "timestamps": metrics.gpu_monitor.timestamps if metrics.gpu_monitor else []
            }
        }, f, indent=2)
    
    # Generate plots
    plt.figure(figsize=(20, 12))
    plt.suptitle(f"{MODEL_NAME} - {metrics.scenario_name} (Concurrency: {metrics.concurrency})", 
                fontsize=14, y=1.02)
    
    # TTFT Plot
    plt.subplot(2, 3, 1)
    plt.hist(metrics.ttft_list, bins=10, color='skyblue', edgecolor='black')
    plt.title("TTFT Distribution", pad=10)
    plt.xlabel("Time (seconds)")
    plt.ylabel("Frequency")
    plt.grid(True, alpha=0.3)
    
    # TBT Plot
    plt.subplot(2, 3, 2)
    plt.hist(metrics.tbt_list, bins=10, color='lightgreen', edgecolor='black')
    plt.title("TBT Distribution", pad=10)
    plt.xlabel("Time between tokens (seconds)")
    plt.ylabel("Frequency")
    plt.grid(True, alpha=0.3)
    
    # TPS Plot
    plt.subplot(2, 3, 3)
    plt.plot(metrics.tps_list, marker='o', linestyle='-', color='salmon')
    plt.title("TPS Trend", pad=10)
    plt.xlabel("Request sequence")
    plt.ylabel("Tokens per second")
    plt.grid(True, alpha=0.3)
    
    # 百分位数据表格
    plt.subplot(2, 3, 4)
    plt.axis('off')
    table_data = [
        ["Metric", "P50", "P90", "P95", "P99", "Mean"],
        ["TTFT (s)", 
         f"{ttft_percentiles['p50']:.4f}", 
         f"{ttft_percentiles['p90']:.4f}", 
         f"{ttft_percentiles['p95']:.4f}", 
         f"{ttft_percentiles['p99']:.4f}", 
         f"{ttft_percentiles['mean']:.4f}"],
        ["TBT (s)", 
         f"{tbt_percentiles['p50']:.4f}", 
         f"{tbt_percentiles['p90']:.4f}", 
         f"{tbt_percentiles['p95']:.4f}", 
         f"{tbt_percentiles['p99']:.4f}", 
         f"{tbt_percentiles['mean']:.4f}"],
        ["TPS (t/s)", 
         f"{tps_percentiles['p50']:.2f}", 
         f"{tps_percentiles['p90']:.2f}", 
         f"{tps_percentiles['p95']:.2f}", 
         f"{tps_percentiles['p99']:.2f}", 
         f"{tps_percentiles['mean']:.2f}"]
    ]
    plt.table(cellText=table_data, loc='center', cellLoc='center', colWidths=[0.2]*6)
    plt.title("Performance Percentiles", pad=20)
    
    # GPU监控图表
    if metrics.gpu_monitor:
        plt.subplot(2, 3, 5)
        plt.plot(metrics.gpu_monitor.timestamps, metrics.gpu_monitor.gpu_utilizations, 'r-')
        plt.title("GPU Utilization (%)", pad=10)
        plt.xlabel("Time")
        plt.ylabel("Utilization %")
        plt.grid(True, alpha=0.3)
        
        plt.subplot(2, 3, 6)
        plt.plot(metrics.gpu_monitor.timestamps, metrics.gpu_monitor.gpu_memories, 'b-')
        plt.title("GPU Memory Usage (MB)", pad=10)
        plt.xlabel("Time")
        plt.ylabel("Memory (MB)")
        plt.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig(f"{base_filename}.png", bbox_inches='tight', dpi=300)
    plt.close()
    
    # Print summary
    print(f"\n{metrics.scenario_name.upper()} RESULTS ({MODEL_NAME})")
    print(f"Concurrency: {metrics.concurrency}")
    print(f"Avg TTFT: {ttft_percentiles['mean']:.4f}s (P99: {ttft_percentiles['p99']:.4f}s)")
    print(f"Avg TBT: {tbt_percentiles['mean']:.4f}s (P99: {tbt_percentiles['p99']:.4f}s)")
    print(f"Avg TPS: {tps_percentiles['mean']:.2f} tokens/s (P99: {tps_percentiles['p99']:.2f}t/s)")
    print(f"Total tokens: {metrics.total_tokens}")
    if metrics.gpu_monitor:
        print(f"Max GPU Util: {max(metrics.gpu_monitor.gpu_utilizations)}%")
        print(f"Max GPU Mem: {max(metrics.gpu_monitor.gpu_memories):.2f}MB")
    print(f"Results saved to {base_filename}.[json/png]")

    # Print summary To File
    summary_filename = f"{base_filename}_summary.txt"
    with open(summary_filename, "w") as f:
        f.write(f"{metrics.scenario_name.upper()} RESULTS ({MODEL_NAME})\n")
        f.write(f"Concurrency: {metrics.concurrency}\n")
        f.write(f"Avg TTFT: {ttft_percentiles['mean']:.4f}s (P99: {ttft_percentiles['p99']:.4f}s)\n")
        f.write(f"Avg TBT: {tbt_percentiles['mean']:.4f}s (P99: {tbt_percentiles['p99']:.4f}s)\n")
        f.write(f"Avg TPS: {tps_percentiles['mean']:.2f} tokens/s (P99: {tps_percentiles['p99']:.2f}t/s)\n")
        f.write(f"Total tokens: {metrics.total_tokens}\n")
        if metrics.gpu_monitor:
            f.write(f"Max GPU Util: {max(metrics.gpu_monitor.gpu_utilizations)}%\n")
            f.write(f"Max GPU Mem: {max(metrics.gpu_monitor.gpu_memories):.2f}MB\n")
        f.write(f"Results saved to {summary_filename}\n")

    plt.close('all')
    
    
def main():
    requests_num = 40

    # Define test scenarios
    scenarios = [
        {"type": "short_text", "concurrency": 5, "max_tokens": 20, "requests": requests_num},
        {"type": "qa_medium", "concurrency": 3, "max_tokens": 100, "requests": requests_num},
        {"type": "long_form", "concurrency": 1, "max_tokens": 300, "requests": requests_num},
        {"type": "stress_test", "concurrency": 10, "max_tokens": 50, "requests": requests_num},
        {"type": "mixed_load", "concurrency": 4, "max_tokens": 200, "requests": requests_num}
    ]

    # Run all scenarios
    for scenario in scenarios:
        print(f"\nStarting {scenario['type']} scenario...")
        metrics = run_scenario(
            scenario_type=scenario["type"],
            concurrency=scenario["concurrency"],
            max_tokens=scenario["max_tokens"],
            num_requests=scenario["requests"]
        )
        save_results(metrics)

if __name__ == "__main__":
    main()
    # 确保NVML被正确关闭
    if gpu_available:
        pynvml.nvmlShutdown()