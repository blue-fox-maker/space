#pragma once
#include <algorithm>
#include <concepts>
#include <functional>
#include <iostream>
#include <numeric>
#include <ostream>
#include <ranges>
#include <vector>
#include <map>
#include <set>

template <typename T>
class disjoint_set {
  using value_t = T;
  using size_type = size_t;
  std::map<value_t,value_t> parent;

public:
  disjoint_set()=default;
  template <std::ranges::range R>
  disjoint_set(R&& rng){
    for (auto && sub_rng: rng)
      merge(sub_rng);
  }
  value_t find(const value_t &x) {
    auto iter = parent.find(x);
    if(iter==parent.end())
    {
      parent[x]=x;
      return x;
    }
    return x == iter->second ? x: (iter->second = find(iter->second));
  }
  value_t find(const value_t &x) const {
    auto iter = parent.find(x);
    if (iter==parent.end())
      return x;
    return x == iter->second ? x: find(iter->second);
  }
  void merge(const value_t& x, const value_t &y){
    parent[find(x)] = find(y);
  }
  template <std::ranges::range R>
  void merge(R&& rng){
    std::ranges::for_each(std::forward<R>(rng)|std::views::pairwise,[this](auto x){merge(x.first,x.second);});
  }
  size_t size() const { return parent.size();}

  explicit operator std::set<std::set<value_t>>()
  {
    std::set<std::set<value_t>> result;
    std::map<value_t,std::set<value_t>> temp;
    for (auto&&x:parent|std::views::keys)    
      temp[find(x)].insert(x);
    for (auto&&[k,v]:temp)
      result.insert(std::move(v));
    return result;
  } 
};

