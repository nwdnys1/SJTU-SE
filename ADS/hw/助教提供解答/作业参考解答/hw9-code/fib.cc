#include <thread>
#include <print>
#include <cstdint>
#include <chrono>
#include <cassert>

uint64_t fib_seq(int n)
{
	if (n <= 1)
		return 1;
	return fib_seq(n-1) + fib_seq(n-2);
}

uint64_t fib_parallel(int n, int max_thread)
{
	if (max_thread == 1)
		return fib_seq(n);
	if (n <= 1)
		return 1;
	uint64_t f1;
	int half_thread = (max_thread + 1) / 2;
	std::thread t([&, n, half_thread]() { f1 = fib_parallel(n-1, half_thread); });
	auto f2 = fib_parallel(n-2, max_thread - half_thread);
	t.join();
	return f1 + f2;
}

int main(int argc, char **argv)
{
	assert(argc == 2);
	int thread_num = atoi(argv[1]);
	using clock = std::chrono::system_clock;
	auto t1 = clock::now();
	auto ans1 = fib_seq(40);
	auto t2 = clock::now();
	auto ans2 = fib_parallel(40, thread_num);
	auto t3 = clock::now();
	auto time_seq = t2-t1, time_par = t3-t2;
//	std::print(" | sequential | parallel ({})\n"
//			"--|--|--\n"
//			"ans | {} | {}\n"
//			"time | {} | {}\n\n",
//			thread_num,
//			ans1, ans2, time_seq, time_par);
	assert(ans1 == ans2);
	std::print("{:.2}\n", double(time_seq.count()) / time_par.count());
}
