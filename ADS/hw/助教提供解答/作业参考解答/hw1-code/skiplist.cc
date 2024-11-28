#include "skiplist.h"
#include <vector>

namespace skiplist {


skiplist_type::skiplist_type(double p) : rand_p(1-p),
	top_head(new skiplist_node(nullptr, nullptr, {}, {}))
{
}
void skiplist_type::put(key_type key, const value_type &val)
{
	std::vector<skiplist_node*> query_path;
	auto n = this->top_head;
	while (true)
	{
		while (n->right != nullptr && n->right->key < key)
			n = n->right;
		query_path.push_back(n);
		if (n->down == nullptr)
			break;
		n = n->down;
	}
	int level = this->rand_p(this->rng);
	skiplist_node *new_node = nullptr;
	while (level >= 0)
	{
		auto left = query_path.back();
		query_path.pop_back();
		new_node = new skiplist_node(left->right, new_node, key, val);
		left->right = new_node;
		--level;
		if (query_path.empty())
			break;
	}
	while (level >= 0)
	{
		new_node = new skiplist_node(nullptr, new_node, key, val);
		this->top_head = new skiplist_node(new_node, this->top_head, {}, {});
		--level;
	}
}
std::string skiplist_type::get(key_type key) const
{
	auto n = this->top_head;
	while (true)
	{
		while (n->right != nullptr && n->right->key < key)
			n = n->right;
		if (n->right != nullptr && n->right->key == key)
			return n->right->val;
		if (n->down == nullptr)
			break;
		n = n->down;
	}
	return {};
}
int skiplist_type::query_distance(key_type key) const
{
	int ret = 1;
	auto n = this->top_head;
	while (true)
	{
		while (n->right != nullptr && n->right->key < key)
		{
			n = n->right;
			++ret;
		}
		if (n->right != nullptr && n->right->key == key)
			return ret + 1;
		if (n->down == nullptr)
			break;
		n = n->down;
		++ret;
	}
	return ret;
}

skiplist_type::~skiplist_type()
{
	auto head = this->top_head;
	while (head != nullptr)
	{
		auto d = head->down;
		auto n = head;
		while (n != nullptr)
		{
			auto r = n->right;
			delete n;
			n = r;
		}
		head = d;
	}
}

} // namespace skiplist
