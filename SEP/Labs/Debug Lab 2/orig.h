#pragma once

#include <algorithm>
#include <iostream>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

class node_t {
public:
  node_t(node_t *parent = nullptr, bool is_leaf = false,
         node_t *prev_ = nullptr, node_t *next_ = nullptr)
      : up(parent), left(prev_), right(next_), is_leaf(is_leaf) {
    if (next_) {
      next_->left = this;
    }

    if (prev_) {
      prev_->right = this;
    }
  }

  ~node_t() {
    for (node_t *child : down) {
      delete child;
    }
  }

  std::vector<int> key_list;
  std::vector<int> value_list;

  node_t *up;
  std::vector<node_t *> down;

  node_t *left;
  node_t *right;

  bool is_leaf;

  int index_of_child(int key) {
    for (int i = 0; i < key_list.size(); i++) {
      if (key < key_list[i]) {
        return i;
      }
    }
    return key_list.size();
  }

  int index_of_key(int key) {
    for (int i = 0; i < key_list.size(); i++) {
      if (key == key_list[i]) {
        return i;
      }
    }
    return -1;
  }

  node_t *get_child(int key) { return down[index_of_child(key)]; }

  void set_child(int key, std::vector<node_t *> value) {
    int i = index_of_child(key);
    key_list.insert(key_list.begin() + i, key);
    down.erase(down.begin() + i);
    down.insert(down.begin() + i, value.begin(), value.end());
  }

  std::tuple<int, node_t *, node_t *> split_internal() {
    node_t *left = new node_t(up, false, nullptr, nullptr);
    int mid = key_list.size() / 2;

    copy(key_list.begin(), key_list.begin() + mid,
         back_inserter(left->key_list));
    copy(down.begin(), down.begin() + mid + 1, back_inserter(left->down));

    for (node_t *child : left->down) {
      child->up = left;
    }

    int key = key_list[mid];
    key_list.erase(key_list.begin(), key_list.begin() + mid + 1);
    down.erase(down.begin(), down.begin() + mid + 1);

    return {key, left, this};
  }

  int get(int key) {
    int index = -1;
    for (int i = 0; i < key_list.size(); ++i) {
      if (key_list[i] == key) {
        index = i;
        break;
      }
    }

    if (index == -1) {
      throw std::invalid_argument("key not found");
    }

    return value_list[index];
  }

  void set(int key, int value) {
    int i = index_of_child(key);
    if (std::find(key_list.begin(), key_list.end(), key) == key_list.end()) {
      key_list.insert(key_list.begin() + i, key);
      value_list.insert(value_list.begin() + i, value);
    } else {
      value_list[i - 1] = value;
    }
  }

  std::tuple<int, node_t *, node_t *> split_leaf() {
    node_t *left = new node_t(up, true, left, this);
    int mid = key_list.size() / 2;

    left->key_list = std::vector<int>(key_list.begin(), key_list.begin() + mid);
    left->value_list =
        std::vector<int>(value_list.begin(), value_list.begin() + mid);

    key_list.erase(key_list.begin(), key_list.begin() + mid);
    value_list.erase(value_list.begin(), value_list.begin() + mid);

    return {key_list[0], left, this};
  }
};

class tree_t {
public:
  tree_t(int max_capacity_ = 4) {
    root = new node_t(nullptr, true, nullptr, nullptr);
    max_capacity = max_capacity_ > 2 ? max_capacity_ : 2;
    min_capacity = max_capacity / 2;
    height = 0;
  }

  ~tree_t() { delete root; }

  node_t *root;
  int max_capacity;
  int min_capacity;
  int height;

  node_t *find_leaf(int key) {
    node_t *node = root;
    while (node->is_leaf) {
      node = node->get_child(key);
    }
    return node;
  }

  int get(int key) { return find_leaf(key)->get(key); }

  void set(int key, int value) {
    node_t *leaf = find_leaf(key);
    leaf->set(key, value);
    if (leaf->key_list.size() > max_capacity) {
      insert(leaf->split_leaf());
    }
  }

  void insert(std::tuple<int, node_t *, node_t *> result) {
    auto [key, left, right] = result;
    node_t *parent = right->up;
    if (parent == nullptr) {
      left->up = right->up = root =
          new node_t(nullptr, false, nullptr, nullptr);
      height += 1;
      root->key_list = {key};
      root->down = {left, right};
      return;
    }
    parent->set_child(key, {left, right});
    if (parent->key_list.size() > max_capacity) {
      insert(parent->split_internal());
    }
  }

  void remove_from_leaf(int key, node_t *node) {
    int index = node->index_of_key(key);
    if (index == -1) {
      throw std::invalid_argument("key not found");
    }
    node->key_list.erase(node->key_list.begin() + index);
    node->value_list.erase(node->value_list.begin() + index);
    if (node->up) {
      int index_in_parent = node->up->index_of_child(key);
      if (index_in_parent)
        node->up->key_list[index_in_parent - 1] = node->key_list.front();
    }
  }

  void remove_from_internal(int key, node_t *node) {
    int index = node->index_of_key(key);
    if (index != -1) {
      node_t *left_most_leaf = node->down[index + 1];
      while (!left_most_leaf->is_leaf)
        left_most_leaf = left_most_leaf->down.front();

      node->key_list[index] = left_most_leaf->key_list.front();
    }
  }

  void borrow_key_from_right_leaf(node_t *node, node_t *next) {
    node->key_list.push_back(next->key_list.front());
    next->key_list.erase(next->key_list.begin());
    node->value_list.push_back(next->value_list.front());
    next->value_list.erase(next->value_list.begin());
    for (int i = 0; i < node->up->down.size(); i++) {
      if (node->up->down[i] == next) {
        node->up->key_list[i - 1] = next->key_list.front();
        break;
      }
    }
  }

  void borrow_key_from_left_leaf(node_t *node, node_t *prev) {
    node->key_list.insert(node->key_list.begin(), prev->key_list.back());
    prev->key_list.erase(prev->key_list.end() - 1);
    node->value_list.insert(node->value_list.begin(), prev->value_list.back());
    prev->value_list.erase(prev->value_list.end() - 1);
    for (int i = 0; i < node->up->down.size(); i++) {
      if (node->up->down[i] == node) {
        node->up->key_list[i - 1] = node->key_list.front();
        break;
      }
    }
  }

  void merge_node_with_right_leaf(node_t *node, node_t *next) {
    node->key_list.insert(node->key_list.end(), next->key_list.begin(),
                          next->key_list.end());
    node->value_list.insert(node->value_list.end(), next->value_list.begin(),
                            next->value_list.end());
    node->right = next->right;
    if (node->right)
      node->right->left = node;
    for (int i = 0; i < next->up->down.size(); i++) {
      if (node->up->down[i] == next) {
        node->up->key_list.erase(node->up->key_list.begin() + i - 1);
        node->up->down.erase(node->up->down.begin() + i);

        break;
      }
    }
  }

  void merge_node_with_left_leaf(node_t *node, node_t *prev) {
    prev->key_list.insert(prev->key_list.end(), node->key_list.begin(),
                          node->key_list.end());
    prev->value_list.insert(prev->value_list.end(), node->value_list.begin(),
                            node->value_list.end());

    prev->right = node->right;
    if (prev->right)
      prev->right->left = prev;

    for (int i = 0; i < node->up->down.size(); i++) {
      if (node->up->down[i] == node) {
        node->up->key_list.erase(node->up->key_list.begin() + i - 1);
        node->up->down.erase(node->up->down.begin() + i);
        break;
      }
    }
  }

  void borrow_key_from_right_internal(int my_position_in_parent, node_t *node,
                                      node_t *next) {
    node->key_list.insert(node->key_list.end(),
                          node->up->key_list[my_position_in_parent]);
    node->up->key_list[my_position_in_parent] = next->key_list.front();
    next->key_list.erase(next->key_list.begin());
    node->down.insert(node->down.end(), next->down.front());
    next->down.erase(next->down.begin());
    node->down.back()->up = node;
  }

  void borrow_key_from_left_internal(int my_position_in_parent, node_t *node,
                                     node_t *prev) {
    node->key_list.insert(node->key_list.begin(),
                          node->up->key_list[my_position_in_parent - 1]);
    node->up->key_list[my_position_in_parent - 1] = prev->key_list.back();
    prev->key_list.erase(prev->key_list.end() - 1);
    node->down.insert(node->down.begin(), prev->down.back());
    prev->down.erase(prev->down.end() - 1);
    node->down.front()->up = node;
  }

  void merge_node_with_right_internal(int my_position_in_parent, node_t *node,
                                      node_t *next) {
    node->key_list.insert(node->key_list.end(),
                          node->up->key_list[my_position_in_parent]);
    node->up->key_list.erase(node->up->key_list.begin() +
                             my_position_in_parent);
    node->up->down.erase(node->up->down.begin() + my_position_in_parent + 1);
    node->key_list.insert(node->key_list.end(), next->key_list.begin(),
                          next->key_list.end());
    node->down.insert(node->down.end(), next->down.begin(), next->down.end());
    for (node_t *child : node->down) {
      child->up = node;
    }
    delete next;
  }

  void merge_node_with_left_internal(int my_position_in_parent, node_t *node,
                                     node_t *prev) {
    prev->key_list.insert(prev->key_list.end(),
                          node->up->key_list[my_position_in_parent - 1]);
    node->up->key_list.erase(node->up->key_list.begin() +
                             my_position_in_parent - 1);
    node->up->down.erase(node->up->down.begin() + my_position_in_parent);
    prev->key_list.insert(prev->key_list.end(), node->key_list.begin(),
                          node->key_list.end());
    prev->down.insert(prev->down.end(), node->down.begin(), node->down.end());
    for (node_t *child : prev->down) {
      child->up = prev;
    }
    delete node;
  }

  void remove(int key, node_t *node = nullptr) {
    if (node == nullptr) {
      node = find_leaf(key);
    }
    if (node->is_leaf) {
      remove_from_leaf(key, node);
    } else {
      remove_from_internal(key, node);
    }

    if (node->key_list.size() < min_capacity) {
      if (node == root) {
        if (root->key_list.empty() && !root->down.empty()) {
          root = root->down[0];
          root->up = nullptr;
          height -= 1;
        }
        return;
      }

      else if (node->is_leaf) {
        node_t *next = node->right;
        node_t *prev = node->left;

        if (next && next->up == node->up &&
            next->key_list.size() > min_capacity) {
          borrow_key_from_right_leaf(node, next);
        } else if (prev && prev->up == node->up &&
                   prev->key_list.size() > min_capacity) {
          borrow_key_from_left_leaf(node, prev);
        } else if (next && next->up == node->up &&
                   next->key_list.size() <= min_capacity) {
          merge_node_with_right_leaf(node, next);
        } else if (prev && prev->up == node->up &&
                   prev->key_list.size() <= min_capacity) {
          merge_node_with_left_leaf(node, prev);
        }
      } else {
        int my_position_in_parent = -1;

        for (int i = 0; i < node->up->down.size(); i++) {
          if (node->up->down[i] == node) {
            my_position_in_parent = i;
            break;
          }
        }

        node_t *next;
        node_t *prev;

        if (node->up->down.size() > my_position_in_parent + 1) {
          next = node->up->down[my_position_in_parent + 1];
        } else {
          next = nullptr;
        }

        if (my_position_in_parent) {
          prev = node->up->down[my_position_in_parent - 1];
        } else {
          prev = nullptr;
        }

        if (next && next->up == node->up &&
            next->key_list.size() > min_capacity) {
          borrow_key_from_right_internal(my_position_in_parent, node, next);
        }

        else if (prev && prev->up == node->up &&
                 prev->key_list.size() > min_capacity) {
          borrow_key_from_left_internal(my_position_in_parent, node, prev);
        }

        else if (next && next->up == node->up &&
                 next->key_list.size() <= min_capacity) {
          merge_node_with_right_internal(my_position_in_parent, node, next);
        }

        else if (prev && prev->up == node->up &&
                 prev->key_list.size() <= min_capacity) {
          merge_node_with_left_internal(my_position_in_parent, node, prev);
        }
      }
    }
    if (node->up) {
      remove(key, node->up);
    }
  }
};
