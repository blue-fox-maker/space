#include "dbg.h"
#include "disjoint_set.hpp"
#include "index.hpp"
#include "range_tree.hpp"
#include <algorithm>
#include <concepts>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <numeric>
#include <ostream>
#include <ranges>
#include <set>
#include <string>
#include <variant>
#include "rich.hpp"

class LneIndex {
  using value_t = size_t;
  using stamp_t = size_t;
  std::function<bool(const stamp_t, const stamp_t, const value_t, const value_t)> is_equivalent;
  std::vector<stamp_t> stamps;
  std::vector<value_t> values;
  size_t num_value;
  size_t num_stamp;
  std::map<std::pair<stamp_t, stamp_t>, std::vector<std::tuple<value_t, value_t, stamp_t>>> m_data;

public:
  size_t size(){return std::accumulate(m_data.begin(),m_data.end(),0,[](const auto &val,const auto &x){return val+x.second.size();});}
  LneIndex(const size_t num_value, const size_t num_stamp, const std::function<bool(const stamp_t, const stamp_t, const value_t, const value_t)> &is_equivalent) : num_stamp(num_stamp), num_value(num_value), is_equivalent(is_equivalent), stamps(num_stamp), values(num_value) {
    using A_type = std::tuple<value_t, value_t, stamp_t, stamp_t>;
    using B_type = std::tuple<value_t, value_t, stamp_t, stamp_t, stamp_t>;
    std::iota(stamps.begin(), stamps.end(), 0);
    std::iota(values.begin(), values.end(), 0);
    std::set<B_type> items;
    std::map<std::pair<stamp_t, stamp_t>, disjoint_set<value_t>> equivalence_class;
    for (value_t u: std::views::iota(0,int(num_value))|track())
    // for (value_t u = 0; u < num_value; u++)
    {
      for (value_t v = u + 1; v < num_value; v++) {
        for (auto iter1 = stamps.begin(); iter1 != stamps.end(); iter1++) {
          auto iter3 = std::ranges::find_if(iter1, stamps.end(), [&](auto t3) { return is_equivalent(*iter1, t3, u, v); });
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
    std::cout<<std::endl;
    auto trivial_partition = disjoint_set<value_t>{num_value};
    auto id_A = index<A_type, value_t>{};
    auto id_B = index<B_type, value_t>{};
    auto o_adj = std::vector<std::set<value_t>>(items.size());
    size_t count = 0;
    std::cout<<std::endl;
    for (auto [u, v, t1, t2, t3] : items) {
      std::cout<<"["<<count++<<"/"<<items.size()<<"]\n";
      for (auto ts = t1; ts <= t2; ts++) {
        auto &prev_partition = equivalence_class.contains({ts, t3 - 1}) ? equivalence_class[{ts, t3 - 1}] : trivial_partition;
        auto a = id_A[{prev_partition.find(u), prev_partition.find(v), ts, t3}];
        auto b = id_B[{u, v, t1, t2, t3}];
        o_adj[b].insert(a);
      }
    }
    auto i_adj = std::vector<std::vector<value_t>>(id_A.size());
    for (value_t b = 0; b < id_B.size(); b++)
      for (auto a : o_adj[b])
        i_adj[a].push_back(b);
    auto is_dominated = std::vector<bool>(id_A.size());
    while (!std::ranges::all_of(is_dominated, std::identity{})) {
      auto cur_b = std::ranges::max(std::views::iota(0) | std::views::take(id_B.size()), {}, [&](auto x) { return o_adj[x].size(); });
      auto [u, v, t1, t2, t3] = id_B[cur_b];
      m_data[{t1, t2}].push_back({u, v, t3});
      for (auto a : o_adj[cur_b]) {
        is_dominated[a] = true;
        for (auto &&b : i_adj[a]) {
          if (b != cur_b) {
            o_adj[b].erase(a);
          }
        }
      }
      o_adj[cur_b].clear();
    }
    dbg(m_data);
  }
  disjoint_set<value_t> query(stamp_t ts, stamp_t te) {
    auto result = disjoint_set<value_t>{num_value};
    for (auto [k, values] : m_data) {
      auto [t1, t2] = k;
      if (t1 <= ts && ts <= t2) {
        for (auto [u, v, t3] : values) {
          if (te >= t3)
            result.merge(u, v);
        }
      }
    }
    return result;
  }
  void display() {
    dbg(m_data);
  }
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
  size_t size(){return std::accumulate(m_data.begin(),m_data.end(),0,[](const auto &val,const auto &x){return val+x.size();});}
  PhcIndex(const size_t num_value, const size_t num_stamp, const std::function<bool(const stamp_t, const stamp_t, const value_t)> &unary_pred) : num_stamp(num_stamp), num_value(num_value), stamps(num_stamp), values(num_value), m_data(num_value) {
    std::iota(stamps.begin(), stamps.end(), 0);
    std::iota(values.begin(), values.end(), 0);
    bool prev_res = false;
    for (auto v : values) {
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
      m_data[v].push_back({*iter1,num_stamp});
    }
    dbg(m_data);
  }
  std::set<value_t> query(stamp_t ts, stamp_t te)
  {
    std::set<value_t> result;
    for(auto v:values)
    {
      auto iter = std::ranges::lower_bound(m_data[v]|std::views::reverse,ts,std::greater{},[](const auto& x){return x.first;});
      if(te>=iter->second)
        result.insert(v);
    }
    return result;
  }
};
