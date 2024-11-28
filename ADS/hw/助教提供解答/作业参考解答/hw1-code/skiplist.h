#ifndef SKIPLIST_H
#define SKIPLIST_H

#include <cstdint>
#include <string>
#include <random>

namespace skiplist {
using key_type = uint64_t;
using value_type = std::string;

struct skiplist_node
{
	skiplist_node *right = nullptr, *down = nullptr;
	key_type key;
	value_type val;
	skiplist_node(skiplist_node *r, skiplist_node *d, key_type k, value_type v) :
		right(r), down(d), key(k), val(v) {}
};

class skiplist_type
{
	std::mt19937_64 rng{1234};
	std::geometric_distribution<int> rand_p;
	skiplist_node *top_head;
public:
	explicit skiplist_type(double p = 0.5);
	void put(key_type key, const value_type &val);
	std::string get(key_type key) const;

	// for hw1 only
	int query_distance(key_type key) const;
	~skiplist_type();
};

} // namespace skiplist

#endif // SKIPLIST_H
