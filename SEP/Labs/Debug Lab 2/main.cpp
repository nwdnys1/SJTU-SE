#include "tree.h"

#include <cassert>

int main() {
  tree_t tree(3);
  tree.set(1, 42);
  auto v = tree.get(1);
  assert(v == 42);
  tree.remove(1);

  return 0;
}
