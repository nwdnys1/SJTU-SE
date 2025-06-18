import openai
import time
import asyncio
import aiohttp
from typing import List, Dict
import statistics
import matplotlib.pyplot as plt

# Configure OpenAI client
client = openai.OpenAI(
    base_url="http://localhost:11434/v1",
    api_key="no-key-required"
)

class PerformanceMetrics:
    def __init__(self):
        self.ttft_list = []    # Time-To-First-Token
        self.tbt_list = []     # Time-Between-Tokens
        self.tps_list = []     # Tokens-Per-Second
        self.total_tokens = 0

async def test_chat_completion(
    prompt: str,
    max_tokens: int,
    concurrency: int = 1,
    num_requests: int = 10
) -> PerformanceMetrics:
    """Test chat completion performance (compatible with synchronous streams)"""
    metrics = PerformanceMetrics()
    semaphore = asyncio.Semaphore(concurrency)

    async def make_request():
        async with semaphore:
            start_time = time.time()
            
            # Using synchronous stream processing
            stream = client.chat.completions.create(
                model="Qwen/Qwen3-0.6B",
                messages=[{"role": "user", "content": prompt}],
                max_tokens=max_tokens,
                stream=True
            )
            
            first_token = True
            token_count = 0
            last_time = start_time
            
            # Synchronous iterator processing
            for chunk in stream:
                if first_token:
                    metrics.ttft_list.append(time.time() - start_time)
                    first_token = False
                    last_time = time.time()
                else:
                    current_time = time.time()
                    metrics.tbt_list.append(current_time - last_time)
                    last_time = current_time
                token_count += 1
            
            metrics.total_tokens += token_count
            duration = time.time() - start_time
            if duration > 0:
                metrics.tps_list.append(token_count / duration)

    tasks = [make_request() for _ in range(num_requests)]
    await asyncio.gather(*tasks)
    return metrics

def print_metrics(metrics: PerformanceMetrics, test_name: str):
    """Print performance metrics"""
    print(f"\n{test_name} Test Results:")
    print(f"Average TTFT: {statistics.mean(metrics.ttft_list):.3f}s")
    print(f"Average TBT: {statistics.mean(metrics.tbt_list):.3f}s")
    print(f"Average TPS: {statistics.mean(metrics.tps_list):.2f} tokens/s")
    print(f"Total tokens generated: {metrics.total_tokens}")

def plot_results(metrics: PerformanceMetrics, test_name: str):
    """Plot performance charts"""
    plt.figure(figsize=(12, 4))
    
    plt.subplot(1, 3, 1)
    plt.hist(metrics.ttft_list, bins=10)
    plt.title(f"{test_name} - TTFT Distribution")
    plt.xlabel("Time (s)")
    plt.ylabel("Frequency")
    
    plt.subplot(1, 3, 2)
    plt.hist(metrics.tbt_list, bins=10)
    plt.title("TBT Distribution")
    plt.xlabel("Time (s)")
    plt.ylabel("Frequency")
    
    plt.subplot(1, 3, 3)
    plt.plot(metrics.tps_list)
    plt.title("TPS Trend")
    plt.xlabel("Request Sequence")
    plt.ylabel("Tokens/s")
    
    plt.tight_layout()
    plt.show()

async def main():
    # Define test scenarios
    test_scenarios = [
        {"prompt": "Explain the basic principles of quantum mechanics", "max_tokens": 100, "concurrency": 1},
        {"prompt": "Write a short essay about artificial intelligence", "max_tokens": 200, "concurrency": 5},
        {"prompt": "1+1=", "max_tokens": 10, "concurrency": 10}
    ]

    # Execute tests
    for scenario in test_scenarios:
        metrics = await test_chat_completion(
            prompt=scenario["prompt"],
            max_tokens=scenario["max_tokens"],
            concurrency=scenario["concurrency"],
            num_requests=20
        )
        print_metrics(metrics, f"Scenario: {scenario['prompt'][:20]}...")
        plot_results(metrics, f"Concurrency {scenario['concurrency']}-Length {scenario['max_tokens']}")

if __name__ == "__main__":
    asyncio.run(main())