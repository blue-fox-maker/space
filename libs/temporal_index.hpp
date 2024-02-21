#include "disjoint_set.hpp"
#include "index.hpp"
#include "layered_range_tree.hpp"
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
  // std::map<std::tuple<stamp_t, stamp_t, stamp_t>, std::vector<std::pair<value_t, value_t>>> m_index;
  std::map<std::pair<stamp_t, stamp_t>, std::vector<std::tuple<value_t, value_t,stamp_t>>> m_index;
  RangeTree::RangeTree<stamp_t, std::vector<std::tuple<value_t, value_t, stamp_t>>> m_data;

public:
  size_t size() {
    return std::accumulate(m_index.begin(), m_index.end(), 0, [](const auto &val, const auto &x) { return val + x.second.size(); });
  }
  const auto data() const {
    return m_index;
  }
  LneIndex(const size_type num_value, const size_type num_stamp, const std::map<std::pair<stamp_t, stamp_t>, std::set<std::set<value_t>>> &temporal_communities)
      : num_value(num_value), num_stamp(num_stamp) {
    auto console = rich::console;
    auto partitions = std::map<std::tuple<stamp_t, stamp_t>, disjoint_set<value_t>>{};
    for (auto &&[k, v] : console.track(temporal_communities))
      partitions[k] = v;
    auto E = std::vector<std::tuple<value_t, value_t, stamp_t, stamp_t>>{};
    auto is_equivalent = [&](stamp_t ts, stamp_t te, value_t u, value_t v) { return partitions[{ts, te}].find(u) == partitions[{ts, te}].find(v); };
    auto stamps = std::views::iota(0U, num_stamp);
    for (auto u : console.track(std::views::iota(0U, num_value))) {
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
    for (auto ts : console.track(std::views::iota(0U, num_stamp))) {
      for (auto te : std::views::iota(ts, num_stamp)) {
        for (auto [u, v, t1, t2] : std::ranges::equal_range(E, te, {}, [](auto &x) { return std::get<3>(x); })) {
          if (t1 >= ts) {
            if (current_partition[ts].find(u) != current_partition[ts].find(v)) {
              relation[ts].insert({u, v, t1, t2});
              current_partition[ts].merge(u, v);
              if (ts == 0 || !relation[ts - 1].contains({u, v, t1, t2})) {
                m_index[{ts, t1}].push_back({u, v,t2});
              }
            }
          }
        }
      }
    }
    auto points = std::vector<RangeTree::Point<stamp_t, std::vector<std::tuple<value_t, value_t, stamp_t>>>>{};
    for (auto &&[k, edges] : m_index) {
      auto [t1,t2] = k;
      std::ranges::sort(edges, {}, [](const auto &x) { return std::get<2>(x); });
      // auto point = RangeTree::Point<stamp_t,std::vector<std::pair<value_t,value_t>>>{std::vector{t1,t2,t3},v};
      points.emplace_back(std::vector{t1, t2},edges);
    }
    m_data = RangeTree::RangeTree<stamp_t, std::vector<std::tuple<value_t, value_t, stamp_t>>>{points};
  }
  void query(stamp_t ts, stamp_t te) {
    // auto result = std::vector<value_t>{};
    auto temp = std::vector<bool>(num_value);
    for (auto &&p : m_data.pointsInRange({0, ts}, {ts, te}, {true, true}, {true, true})) {
      const auto &edges = p.value();
      for(auto iter = edges.begin(); iter!= std::ranges::upper_bound(edges,te,{},[](const auto& x){return std::get<2>(x);}); iter++){
        auto [u,v,t3] = *iter;
        if(!temp[u]){}
        if(!temp[v]){}
        temp[u]=true;
        temp[v]=true;
        // result.merge(u,v);
      }
    }
    // return result;
  }
  // disjoint_set<value_t> query(stamp_t ts, stamp_t te) {
  //   auto result = disjoint_set<value_t>{};
  //   for (auto &&p : m_data.pointsInRange({0, ts}, {ts, te}, {true, true}, {true, true})) {
  //     const auto &edges = p.value();
  //     for(auto iter = edges.begin(); iter!= std::ranges::upper_bound(edges,te,{},[](const auto& x){return std::get<2>(x);}); iter++){
  //       auto [u,v,t3] = *iter;
  //       result.merge(u,v);
  //     }
  //   }
  //   return result;
  // }
};

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
    auto console = rich::console();
    std::iota(stamps.begin(), stamps.end(), 0);
    std::iota(values.begin(), values.end(), 0);
    bool prev_res = false;
    for (auto v : console.track(values)) {
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
  void query(stamp_t ts, stamp_t te) {
    // std::vector<value_t> result;
    for (auto v : values) {
      auto iter = std::ranges::lower_bound(m_data[v] | std::views::reverse, ts, std::greater{}, [](const auto &x) { return x.first; });
      if (te >= iter->second)
        {}
        // result.push_back(v);
    }
    // return result;
  }
};
