#include "../libs/index.hpp"
#include "../libs/range_tree.hpp"
#include "../libs/rich.hpp"
#include <algorithm>
#include <cassert>
#include <functional>
#include <map>
#include <ranges>
#include <span>
#include <type_traits>
#include <vector>

template <typename Id, typename Stamp>
struct temporal_index {
  using index_type = Id;

  index_type cur_x = 0;
  index_type cur_y = 0;
  index_type num_vert;

  std::vector<disjoint_set<id_t>> ccs;
  std::vector<std::tuple<index_type, index_type, index_type, index_type, index_type>> index;
  range_tree<std::array<index_type, 3>, std::vector<std::pair<index_type, index_type>>> tree;
  temporal_index(index_type num_vert) : num_vert(num_vert) {}
  void update(index_type u, index_type v, index_type ta, index_type tb) {
    assert((ta <= cur_x && cur_y == tb) || tb > cur_y);
    cur_x = ta;
    cur_y = tb;
    while (ccs.size() <= cur_x)
      ccs.emplace_back(num_vert);
    if (!ccs[cur_x].is_eq(u, v)) {
      index_type t = 0;
      for (index_type i : std::views::iota(0U, cur_x + 1) | std::views::reverse) {
        if (!ccs[i].merge(u, v)) {
          t = i + 1;
          break;
        }
      }
      index.emplace_back(u, v, t, ta, tb);
    }
  }
  void build() {
    auto temp = std::map<std::array<index_type, 3>, std::vector<std::pair<index_type, index_type>>>{};
    for (auto [u, v, t, ta, tb] : index) {
      temp[{t, ta, tb}].push_back({u, v});
    }
    tree = decltype(tree){temp};
  }
  auto query(index_type lower, index_type upper) {
    auto etf = eular_tour_forest<index_type>{num_vert};
    tree.query({0, lower, lower}, {lower, upper, upper}, [&](const decltype(tree)::value_type& edges){
      for(auto [u,v]:edges)
        etf.merge(u,v);
    }); 
    return etf;
  }
  constexpr index_type size() const noexcept { return index.size(); }
};
