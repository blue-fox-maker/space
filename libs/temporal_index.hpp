#include "disjoint_set.hpp"
#include "graph.hpp"
#include "index.hpp"
// #include "pprint.hpp"
#include "rich.hpp"
#include <algorithm>
#include <concepts>
#include <filesystem>
#include <format>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <numeric>
#include <ostream>
#include <ranges>
#include <set>
#include <string>
#include <variant>

template <typename TValue, std::unsigned_integral TIndex>
class index {
  using value_t = TValue;
  using index_t = TIndex;
  std::map<value_t, index_t> v2i;
  std::vector<value_t> i2v;

public:
  [[nodiscard]] index_t size() const { return i2v.size(); }
  value_t operator[](const index_t i) { return i2v[i]; }
  index_t operator[](const value_t &v) {
    if (v2i.contains(v))
      return v2i[v];
    auto i = i2v.size();
    v2i[v] = i;
    i2v.push_back(v);
    return i;
  }
};

class LneIndex {
  using value_t = size_t;
  using stamp_t = size_t;
  using size_type = size_t;
  size_type num_value;
  size_type num_stamp;
  std::map<std::pair<stamp_t, stamp_t>, std::vector<std::tuple<value_t, value_t>>> m_data;
  std::map<std::tuple<stamp_t, stamp_t, stamp_t>, std::vector<std::tuple<value_t, value_t>>> m_index;

public:
  size_t size() {
    return std::accumulate(m_index.begin(), m_index.end(), 0, [](const auto &val, const auto &x) { return val + x.second.size(); });
  }
  const auto data() const {
    return m_index;
  }
  LneIndex(const size_type num_value, const size_type num_stamp, const std::map<std::tuple<stamp_t, stamp_t>, std::set<std::set<value_t>>> &temporal_communities)
      : num_value(num_value), num_stamp(num_stamp) {
    // auto E = std::map<std::pair<stamp_t, stamp_t>, std::set<std::pair<value_t, value_t>>>{};
    auto partitions = std::map<std::tuple<stamp_t, stamp_t>, disjoint_set<value_t>>{};
    for (auto&& [k,v]: temporal_communities|rich::views::track())
      partitions[k]=v;
    auto E = std::vector<std::tuple<value_t, value_t, stamp_t, stamp_t>>{};
    auto is_equivalent = [&](stamp_t ts, stamp_t te, value_t u, value_t v) { return partitions.at({ts, te}).find(u) == partitions.at({ts, te}).find(v); };
    auto stamps = std::views::iota(0U, num_stamp);
    for (auto u : std::views::iota(0U, num_value)|rich::views::track()) {
      for (auto v : std::views::iota(u + 1, num_value)) {
        for (auto iter1 = stamps.begin(); iter1 != stamps.end(); iter1++) {
          auto iter2 = std::ranges::find_if(iter1, stamps.end(), [&](auto t2) { return is_equivalent(*iter1, t2, u, v); });
          if (iter2 != stamps.end()) {
            iter1 = std::ranges::find_if_not(iter1, iter2 + 1, [&](auto t1) { return is_equivalent(t1, *iter2, u, v); }) - 1;
            E.push_back({u, v, *iter1, *iter2});
          } else
            break;
        }
      }
    }
    
    std::ranges::sort(E, {}, [](auto x) {
      auto [u, v, ts, te] = x;
      return std::make_pair(te, te - ts);
    });
    std::vector<disjoint_set<value_t>> current_partition{num_stamp};
    std::vector<std::set<std::tuple<value_t, value_t, stamp_t, stamp_t>>> relation{num_stamp};
    for (auto ts : std::views::iota(0U, num_stamp)|rich::views::track()) {
      for (auto te : std::views::iota(ts, num_stamp)) {
        for (auto [u, v, t1, t2] : std::ranges::equal_range(E,te,{},[](auto &x){return std::get<3>(x);})) {
          if (t1 >= ts ) {
            if (current_partition[ts].find(u)!=current_partition[ts].find(v)){
              relation[ts].insert({u, v, t1, t2});
              current_partition[ts].merge(u, v);
              if (ts == 0 || !relation[ts - 1].contains({u, v, t1, t2})) {
                m_index[{ts, t1, t2}].push_back({u, v});
              }
            }
          }
        }
      }
    }

    // for (auto ts : std::views::iota(0U, num_stamp)) {
    //   for (auto te : std::views::iota(ts, num_stamp)) {
    //     for (auto [interval, edges] : E | std::views::filter([&](auto x) { return x.first.second == te; })) {
    //       for (auto [u, v] : edges) {
    //         if (current_partition[ts].find(u) != current_partition[ts].find(v)) {
    //           current_partition[ts].merge(u, v);
    //           relation[ts].insert({u, v, interval.first, interval.second});
    //           if (ts == 0 || !relation[ts - 1].contains({u, v, interval.first, interval.second})) {
    //             m_data[{ts, te}].push_back({u, v});
    //           }
    //         }
    //       }
    //     }
    //   }
    // }

    //   std::vector<disjoint_set<value_t>> current_partition{num_stamp};
    //   std::vector<std::set<std::tuple<value_t, value_t, stamp_t>>> relation{num_stamp};
    //   for (auto ts : std::views::iota(0U, num_stamp)) {
    //     for (auto te : std::views::iota(ts, num_stamp)) {
    //       for (auto [u, v] : E[{te,te}]) {
    //         if (current_partition[ts].find(u) != current_partition[ts].find(v)) {
    //           current_partition[ts].merge(u, v);
    //           relation[ts].insert({u, v, te});
    //           if (ts == 0 || !relation[ts - 1].contains({u, v, te})) {
    //             m_data[{ts, te}].push_back({u, v});
    //           }
    //         }
    //       }
    //     }
    //   }
  }
  disjoint_set<value_t> query(stamp_t ts, stamp_t te) {
    auto result = disjoint_set<value_t>{};
    // for (auto [k, values] : m_data) {
    //   auto [t1, t2] = k;
    //   if (t1 <= ts && ts <= t2 && t2 <= te) {
    //     for (auto [u, v] : values) {
    //       result.merge(u, v);
    //     }
    //   }
    // }
    for (auto [k, values] : m_index) {
      auto [t1, t2, t3] = k;
      if (t1 <= ts && ts <= t2 && t3 <= te) {
        for (auto [u, v] : values) {
          result.merge(u, v);
        }
      }
    }
    return result;
  }
};

// class LneIndex {
//   using value_t = size_t;
//   using stamp_t = size_t;
//   // std::function<bool(const stamp_t, const stamp_t, const value_t, const value_t)> is_equivalent;
//   size_t num_value;
//   size_t num_stamp;
//   std::map<std::pair<stamp_t, stamp_t>, std::vector<std::tuple<value_t, value_t, stamp_t>>> m_data;

// public:
//   size_t size() {
//     return std::accumulate(m_data.begin(), m_data.end(), 0, [](const auto &val, const auto &x) { return val + x.second.size(); });
//   }
//   const auto data() const {
//     return m_data;
//   }

//   LneIndex(const size_t num_value, const size_t num_stamp, const std::map<std::tuple<stamp_t, stamp_t>, disjoint_set<value_t>> &equivalence_classes) : num_stamp(num_stamp), num_value(num_value) {
//     using A_type = std::tuple<value_t, value_t, stamp_t, stamp_t>;
//     using B_type = std::tuple<value_t, value_t, stamp_t, stamp_t, stamp_t>;
//     std::set<A_type> A_set;
//     std::set<B_type> B_set;
//     std::vector<value_t> values(num_value);
//     std::vector<stamp_t> stamps(num_stamp);
//     std::iota(values.begin(), values.end(), 0);
//     std::iota(stamps.begin(), stamps.end(), 0);
//     auto is_equivalent = [&](const stamp_t ts, const stamp_t te, const value_t u, const value_t v) { return equivalence_classes.at({ts, te}).find(u) == equivalence_classes.at({ts, te}).find(v); };
//     for (value_t u : std::views::iota(0, int(num_value))) {
//       for (value_t v = u + 1; v < num_value; v++) {
//         for (auto iter1 = stamps.begin(); iter1 != stamps.end(); iter1++) {
//           auto iter3 = std::ranges::find_if(iter1, stamps.end(), [&](auto t3) { return is_equivalent(*iter1, t3, u, v); });
//           if (iter3 != stamps.end()) {
//             auto iter2 = std::ranges::find_if_not(iter1, iter3 + 1, [&](auto t2) { return is_equivalent(t2, *iter3, u, v); }) - 1;
//             B_set.emplace(u, v, *iter1, *iter2, *iter3);
//             iter1 = iter2;
//           } else
//             break;
//         }
//       }
//     }
//     auto default_partition = disjoint_set<value_t>{};
//     auto id_A = index<A_type, value_t>{};
//     auto id_B = index<B_type, value_t>{};
//     auto o_adj = std::vector<std::set<value_t>>(B_set.size());
//     size_t count = 0;
//     for (auto [u, v, t1, t2, t3] : B_set) {
//       for (auto ts = t1; ts <= t2; ts++) {
//         auto &prev_partition = ts == t3 ? default_partition : equivalence_classes.at({ts, t3 - 1});
//         auto a = id_A[{prev_partition.find(u), prev_partition.find(v), ts, t3}];
//         auto b = id_B[{u, v, t1, t2, t3}];
//         o_adj[b].insert(a);
//       }
//     }
//     auto i_adj = std::vector<std::vector<value_t>>(id_A.size());
//     for (value_t b = 0; b < id_B.size(); b++)
//       for (auto a : o_adj[b])
//         i_adj[a].push_back(b);
//     auto is_dominated = std::vector<bool>(id_A.size(), false);
//     dbg(i_adj,o_adj);
//     std::map<std::tuple<stamp_t, stamp_t>, disjoint_set<value_t>> current_equivalence_classes;
//     for (auto cur_a : std::views::iota(0, (int)id_A.size()) | std::views::filter([&](auto x) { return !is_dominated[x]; })) {
//       auto [ax, ay, ts, te] = id_A[cur_a];
//       if (current_equivalence_classes[{ts, te}].find(ax) == current_equivalence_classes[{ts, te}].find(ay)) {
//         for (auto b : i_adj[cur_a])
//           o_adj[b].erase(cur_a);
//         continue;
//       }
//       auto cur_b = std::ranges::max(i_adj[cur_a],{}, [&](auto x) {
//         auto [u,v,t1,t2,t3] = id_B[x];
//         return std::make_tuple(o_adj[x].size(),t2-t1); });
//       auto [u, v, t1, t2, t3] = id_B[cur_b];
//       for (auto t = t1; t <= t2; t++)
//         current_equivalence_classes[{t, t3}].merge(u, v);
//       m_data[{t1, t2}].push_back({u, v, t3});
//       for (auto a : o_adj[cur_b]) {
//         is_dominated[a] = true;
//         for (auto b : i_adj[a]) {
//           if (b != cur_b) {
//             o_adj[b].erase(a);
//           }
//         }
//       }
//       o_adj[cur_b].clear();
//       std::cout<<std::format("size of A{}",id_A.size());
//     }
//   }
//   disjoint_set<value_t> query(stamp_t ts, stamp_t te) {
//     auto result = disjoint_set<value_t>{};
//     for (auto [k, values] : m_data) {
//       auto [t1, t2] = k;
//       if (t1 <= ts && ts <= t2) {
//         for (auto [u, v, t3] : values) {
//           if (te >= t3)
//             result.merge(u, v);
//         }
//       }
//     }
//     return result;
//   }
// };

class PhcIndex {
  using value_t = size_t;
  using stamp_t = size_t;
  std::vector<stamp_t> stamps;
  std::vector<value_t> values;
  size_t num_value;
  size_t num_stamp;
  std::vector<std::vector<std::pair<stamp_t, stamp_t>>> m_data;

public:
  size_t size() {
    return std::accumulate(m_data.begin(), m_data.end(), 0, [](const auto &val, const auto &x) { return val + x.size(); });
  }
  const auto data() const {
    return m_data;
  }
  PhcIndex(const size_t num_value, const size_t num_stamp, const std::function<bool(const stamp_t, const stamp_t, const value_t)> &unary_pred) : num_stamp(num_stamp), num_value(num_value), stamps(num_stamp), values(num_value), m_data(num_value) {
    std::iota(stamps.begin(), stamps.end(), 0);
    std::iota(values.begin(), values.end(), 0);
    bool prev_res = false;
    for (auto v : values|rich::views::track()) {
      auto iter1 = stamps.begin(), iter2 = stamps.begin();
      while (iter1 != stamps.end() && iter2 != stamps.end()) {
        bool curr_res = unary_pred(*iter1, *iter2, v);
        if (!curr_res)
          iter2++;
        else {
          if (!prev_res)
            m_data[v].push_back({*iter1, *iter2});
          iter1++;
        }
        prev_res = curr_res;
      }
      m_data[v].push_back({*iter1, num_stamp});
    }
  }
  std::set<value_t> query(stamp_t ts, stamp_t te) {
    std::set<value_t> result;
    for (auto v : values) {
      auto iter = std::ranges::lower_bound(m_data[v] | std::views::reverse, ts, std::greater{}, [](const auto &x) { return x.first; });
      if (te >= iter->second)
        result.insert(v);
    }
    return result;
  }
};
