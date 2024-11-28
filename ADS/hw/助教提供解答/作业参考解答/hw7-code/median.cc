#include <vector>
#include <span>
#include <algorithm>
#include <random>
#include <chrono>
#include <print>
#include <cassert>

using span = std::span<int>;
using span_it = span::iterator;
std::pair<span_it, span_it> partition(span s, int pivot)
{
	std::vector<int> output_buffer(s.size());
	auto less_p = output_buffer.begin(), greater_p = output_buffer.end();
	for (int x : s)
	{
		if (x < pivot)
			*(less_p++) = x;
		else if (x > pivot)
			*(--greater_p) = x;
	}
	auto t1 = std::copy(output_buffer.begin(), less_p, s.begin());
	auto t2 = t1 + (greater_p - less_p);
	std::fill(t1, t2, pivot);
	std::copy(greater_p, output_buffer.end(), t2);
	return {t1, t2};
}

int trivial_nth(span s, int nth)
{
	auto nth_it = s.begin() + nth;
	std::ranges::nth_element(s, s.begin() + nth);
	return *nth_it;
}

int trivial_median(span s)
{
	return trivial_nth(s, s.size() / 2);
}

int linear_nth(span s, int nth, int Q)
{
	assert(nth < ssize(s));
	assert(nth >= 0);
	if (ssize(s) <= Q)
		return trivial_nth(s, nth);
	std::vector<int> mids;
	{
		int i = 0;
		for (; i + Q < ssize(s); i += Q)
			mids.push_back(trivial_median({s.begin() + i, s.begin() + i + Q}));
		mids.push_back(trivial_median({s.begin() + i, s.end()}));
	}
	int pivot = linear_nth(mids, mids.size() / 2, Q);
	auto [less_it, greater_it] = partition(s, pivot);
	int less_cnt = less_it - s.begin();
	if (nth < less_cnt)
		return linear_nth({s.begin(), less_it}, nth, Q);
	int le_cnt = greater_it - s.begin();
	if (nth < le_cnt)
		return pivot;
	return linear_nth({greater_it, s.end()}, nth - le_cnt, Q);
}

int quick_select(span s, int nth, int Q)
{
	assert(nth < ssize(s));
	assert(nth >= 0);
	if (ssize(s) <= Q)
	   return trivial_nth(s, nth);	
	int pivot = s[0];
	auto [less_it, greater_it] = partition(s, pivot);
	int less_cnt = less_it - s.begin();
	if (nth < less_cnt)
		return quick_select({s.begin(), less_it}, nth, Q);
	int le_cnt = greater_it - s.begin();
	if (nth < le_cnt)
		return pivot;
	return quick_select({greater_it, s.end()}, nth - le_cnt, Q);
}

using myclock = std::chrono::system_clock;

myclock::duration test_linear(span s, int nth, int Q)
{
	std::vector<int> copy (s.begin(), s.end());
	auto t1 = myclock::now();
	linear_nth(copy, nth, Q);
	auto t2 = myclock::now();
	return t2 - t1;
}

myclock::duration test_quick(span s, int nth, int Q)
{
	std::vector<int> copy (s.begin(), s.end());
	auto t1 = myclock::now();
	quick_select(copy, nth, Q);
	auto t2 = myclock::now();
	return t2 - t1;
}

void test_with_N(int N)
{
	std::mt19937_64 rng;
	std::vector<int> input_seq(N);
	for (int i = 0; i < N; ++i)
		input_seq[i] = i;
	auto input_rand = input_seq;
	std::ranges::shuffle(input_rand, rng);
	std::uniform_int_distribution<int> rand_nth(0, N-1);
	std::print("N={}:\n", N);
	for (int Q : {5, 6, 7, 8, 9, 10, 11})
	{
		std::vector<myclock::duration> time_seq_linear, time_rand_linear;
		std::vector<myclock::duration> time_seq_quick, time_rand_quick;
		for (int times = 0; times < 1000; ++times)
		{
			int nth = rand_nth(rng);
			time_seq_linear.push_back(test_linear(input_seq, nth, Q));
			time_rand_linear.push_back(test_linear(input_rand, nth, Q));
			time_seq_quick.push_back(test_quick(input_seq, nth, Q));
			time_rand_quick.push_back(test_quick(input_rand, nth, Q));
		}
		std::print("{}", Q);
#define print_p90(t) do {\
			std::ranges::sort(t);\
			std::print("|{}", t[t.size() * 90 / 100]); \
} while (0)
		print_p90(time_seq_linear);
	   	print_p90(time_rand_linear);
		print_p90(time_seq_quick);
		print_p90(time_rand_quick);
#undef print_p90
		std::println();
	}
}
int main()
{
	for (int N : {100, 1000, 2000})
		test_with_N(N);
}
