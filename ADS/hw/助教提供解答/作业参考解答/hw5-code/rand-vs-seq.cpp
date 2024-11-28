#include <print>
#include "rbtree.h"
#include <random>
#include <algorithm>
#include <array>
#include <string_view>
constexpr int N = 10000;
std::array<int, N> seq_input, rand_input;

void test_input(std::string_view msg, std::span<int> inputs)
{
    RedBlackTree tree;
	int color_cnt = 0, rotate_cnt = 0;
	for (int x : inputs)
	{
		auto [c, r] = tree.insert(x);
		color_cnt += c;
		rotate_cnt += r;
	}
	std::print("{}|{}|{}\n", msg, color_cnt, rotate_cnt);
}

int main() {
    RedBlackTree tree;

	for (int i = 0; i < 10000; ++i)
		seq_input[i] = i;
	test_input("顺序", seq_input);
	for (int i = 1; i <= 10; ++i)
	{
		std::mt19937_64 rng(i);
		rand_input = seq_input;
		std::ranges::shuffle(rand_input, rng);
		test_input("乱序", rand_input);
	}
}
