#include "../libs/dbg.h"
#include <fstream>
#include <algorithm>
#include <concepts>
#include <filesystem>
#include <functional>
#include <iostream>
#include <map>
#include <numeric>
#include <ostream>
#include <ranges>
#include <set>
#include <string>
#include <variant>

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

template <typename T>
class graph {
  using vert_t = T;
  using edge_t = std::pair<T, T>;

public:
  std::set<edge_t> edges;
  std::set<vert_t> verts;
  void add_vert(const vert_t &u) {
    verts.insert(u);
  };
  void add_edge(const vert_t &u, const vert_t &v) {
    add_vert(u);
    add_vert(v);
    edges.insert({u, v});
  }
};

template <typename T>
std::ostream &operator<<(std::ostream &os, const graph<T> &G) {
  dbg(G.verts);
  dbg(G.edges);
  return os << "graph with " << G.verts.size() << " verts and " << G.edges.size() << " edges.";
}

class LneIndex {
  using value_t = size_t;
  using stamp_t = size_t;
  std::function<bool(const stamp_t, const stamp_t, const value_t, const value_t)> is_equivalent;
  std::vector<stamp_t> stamps;
  std::vector<value_t> values;
  size_t num_value;
  size_t num_stamp;

public:
  LneIndex(const size_t num_value, const size_t num_stamp, const std::function<bool(const stamp_t, const stamp_t, const value_t, const value_t)> &is_equivalent) : num_stamp(num_stamp), num_value(num_value), is_equivalent(is_equivalent), stamps(num_stamp), values(num_value) {
    using A_type = std::tuple<value_t, value_t, stamp_t, stamp_t>;
    using B_type = std::tuple<value_t, value_t, stamp_t, stamp_t, stamp_t>;
    std::iota(stamps.begin(), stamps.end(), 0);
    std::iota(values.begin(), values.end(), 0);
    std::set<B_type> items;
    std::map<std::pair<stamp_t, stamp_t>, disjoint_set<value_t>> equivalence_class;
    for (value_t u = 0; u < num_value; u++) {
      for (value_t v = u + 1; v < num_value; v++) {
        for (auto iter1 = stamps.begin(); iter1 != stamps.end(); iter1++) {
          decltype(iter1) iter2, iter3;
          iter3 = std::ranges::find_if(iter1, stamps.end(), [&](auto t3) { return is_equivalent(*iter1, t3, u, v); });
          if (iter3 != stamps.end()) {
            auto iter2 = std::ranges::find_if_not(iter1, iter3 + 1, [&](auto t2) { return is_equivalent(t2, *iter3, u, v); }) - 1;
            items.emplace(u, v, *iter1, *iter2, *iter3);
            for (stamp_t ts = *iter1; ts <= *iter2; ts++) {
              if (!equivalence_class.contains({ts, *iter3})) {
                equivalence_class[{ts, *iter3}] = {num_value};
              }
              equivalence_class[{ts, *iter3}].merge(u, v);
            }
            iter1 = iter2;
          } else
            break;
        }
      }
    }
    auto trivial_partition = disjoint_set<value_t>{num_value};
    graph<std::variant<A_type, B_type>> G;
    auto ofs = std::ofstream{"lne_graph.txt",std::ios::out};
    for (auto [u, v, t1, t2, t3] : items) {
      for (auto ts = t1; ts <= t2; ts++) {
        auto prev_partition = equivalence_class.contains({ts, t3 - 1}) ? equivalence_class[{ts, t3 - 1}] : trivial_partition;
        G.add_edge(A_type{prev_partition.find(u), prev_partition.find(v), ts, t3}, B_type{u, v, t1, t2, t3});
        ofs<<prev_partition.find(u)<<" "<<prev_partition.find(v)<<" "<<ts<<" "<<t3<<" "<<u<<" "<<v<<" "<<t1<<" "<<t2<<" "<<t3<<std::endl;
      }
    }
  }
};
