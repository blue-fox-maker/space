#include <cassert>
#include <concepts>
#include <map>
#include <numeric>
#include <vector>
#include <algorithm>
#include <ranges>
#include "disjoint_set.hpp"

class temporal_index {
  using stamp_t = size_t;
  using value_t = size_t;

  std::map<std::tuple<stamp_t,stamp_t,stamp_t>, std::vector<std::pair<value_t,value_t>>> index;

public:
  size_t size() const { return index.size();}
  disjoint_set<value_t> query(stamp_t ts, stamp_t te){
    disjoint_set<value_t> result;
    size_t count = 0;
    for (auto &&[k,edges]: index){
      auto [t1,t2,t3] = k;
      if (t1<=ts&&ts<=t2&&t3<=te)
      for (auto &&edge: edges){
        auto [u,v] = edge;
        result.merge(u,v);
        count ++;
      }
    }
    return result;
  }

  temporal_index(size_t num_value, size_t num_stamp,const std::map<std::pair<stamp_t,stamp_t>, disjoint_set<value_t>> &communities){
    std::vector<std::tuple<stamp_t,stamp_t,value_t,value_t>> E;
    auto is_equivalent = [&](stamp_t ta,stamp_t tb, value_t u, value_t v){ return communities.at({ta,tb}).find(u) == communities.at({ta,tb}).find(v);};
    auto stamps = std::views::iota(0U,num_stamp);
    for (auto u : std::views::iota(0U, num_value)) {
      for (auto v : std::views::iota(u + 1, num_value)) {
        for (auto iter_a = stamps.begin(); iter_a != stamps.end(); iter_a++) {
          auto iter_b = std::ranges::find_if(iter_a, stamps.end(), [=,&is_equivalent](auto tb){return is_equivalent(*iter_a,tb,u,v);});
          if (iter_b != stamps.end()){
            iter_a = std::ranges::find_if_not(iter_a,iter_b+1,[=,&is_equivalent](auto ta){return is_equivalent(ta,*iter_b,u,v);}) -1;
            E.push_back({*iter_a,*iter_b,u,v,});
          } else break;
        }
      }
    }
    auto partitions = std::vector<disjoint_set<value_t>>(num_stamp);    
    auto relation = std::vector<std::set<std::tuple<stamp_t,stamp_t,value_t,value_t>>>(num_stamp);
    // auto index = std::map<std::tuple<stamp_t,stamp_t,stamp_t>, std::vector<std::pair<value_t,value_t>>>{};
    std::ranges::sort(E,{},[](const auto &x){return std::make_pair(std::get<1>(x),std::get<1>(x)-std::get<0>(x));});
    for (auto ts: std::views::iota(0U,num_stamp)){
      for (auto te: std::views::iota(ts,num_stamp)){
        for (auto [t2,t3,u,v]: std::ranges::equal_range(E,te,{},[](const auto &x){return std::get<1>(x);})){
          if (t2 >= ts){
            if (partitions[ts].find(u) != partitions[ts].find(v)){
              partitions[ts].merge(u,v);
              relation[ts].insert({t2,t3,u,v});
              if (ts==0 || !relation[ts-1].contains({t2,t3,u,v})){
                 index[{ts,t2,t3}].push_back({u,v});
              }
            }
          }
        }
      }
    }
  }
};
