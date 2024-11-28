#include "MurmurHash3.h"
#include <print>
#include <functional>
#include <cmath>

double test_false_positive_rate(int m, int n, int k)
{
	std::vector<uint8_t> filter(m);
	std::vector<std::move_only_function<uint32_t(uint32_t)>> hash_functions;
	for (int i = 0; i < k; ++i)
		hash_functions.push_back([=](uint32_t key) {
				std::array<uint32_t, 4> hash_out;
				MurmurHash3_x64_128(&key, sizeof(key), i, hash_out.data());
				return hash_out[0] % m;
		});
	for (int i = 0; i < n; ++i)
		for (auto &h : hash_functions)
			filter[h(i)] = true;
	int false_positive_cnt = 0;
	for (int i = n; i < 2*n; ++i)
	{
		bool is_absent = false;
		for (auto &h : hash_functions)
			if (!filter[h(i)])
			{
				is_absent = true;
				break;
			}
		if (!is_absent)
			++false_positive_cnt;
	}
	return double(false_positive_cnt) / n;
}

int main() {
	constexpr int n = 1024;
	std::print("m/n|opt k");
	for (int k = 1; k <= 5; ++k)
		std::print("|k={}", k);
	std::println();
	std::print("--|--|--|--|--|--|--\n");
	for (int m_n_rate = 2; m_n_rate <= 5; ++m_n_rate)
	{
		std::print("{}|{:.4}", m_n_rate, m_n_rate * M_LN2);
		for (int k = 1; k <= 5; ++k)
			std::print("|{:.4}", test_false_positive_rate(m_n_rate * n, n, k));
		std::println();
	}
    return 0;
}
