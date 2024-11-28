#ifndef SKIPLIST_H
#define SKIPLIST_H

#include <cstdint>
#include <vector>
#include <string>

namespace skiplist {
using key_type = uint64_t;
// using value_type = std::vector<char>;
using value_type = std::string;

struct skiplist_node{
	key_type key;
	value_type val;
	std::vector<skiplist_node*> next;//next[i] is the next node of the current node at level i
	skiplist_node(key_type k, const value_type& v, size_t level) : key(k), val(v), next(level, nullptr) {}
};



class skiplist_type
{
	// add something here
private:
	skiplist_node* head;
	double p;
	int max_level=32;
	int level=1;

	int random_level(){
		int level=1;
		while(((double)rand() / RAND_MAX) < p && level<max_level) level++;
		return level;
	}


public:
	explicit skiplist_type(double p = 0.5);
	void put(key_type key, const value_type &val);
	std::string get(key_type key) const;

	// for hw1 only
	int query_distance(key_type key) const;
};

} // namespace skiplist

#endif // SKIPLIST_H
