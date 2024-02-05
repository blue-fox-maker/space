#include <concepts>
#include <vector>
#include <numeric>

template <std::unsigned_integral T>
class disjoint_set {
private:
  using value_t = T;
  using size_type = T;
  std::vector<value_t> parent;
  std::vector<size_type> rank;

public:
  disjoint_set() = default;
  disjoint_set(size_type size) noexcept : parent(size), rank(size, 1) {
    std::iota(parent.begin(), parent.end(), 0);
  }
  value_t find(value_t x) {
    return x == parent[x] ? x : (parent[x] = find(parent[x]));
  }
  void merge(value_t x, value_t y) {
    x = find(x);
    y = find(y);
    if (rank[x] <= rank[y])
      parent[x] = y;
    else
      parent[y] = x;
    if (rank[x] == rank[y] && x != y)
      rank[y]++;
  }
};

