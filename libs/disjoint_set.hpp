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
  disjoint_set(const std::set<std::set<value_t>>& partitions){
    for (auto&& parts: partitions)
      std::ranges::for_each(parts|std::views::pairwise,[this](auto x){merge(x.first,x.second);});
  }
  disjoint_set(const std::set<value_t>& partitions){
    std::ranges::for_each(partitions|std::views::pairwise,[this](auto x){merge(x.first,x.second);});
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
  void merge(const value_t& x, const value_t &y)
  {
    parent[find(x)] = find(y);
  }
  size_t size() const { return parent.size();}
  // friend disjoint_set<T> operator&(const disjoint_set<T>& a, const disjoint_set<T> &b)
  // {
  //   std::vector<T> values;
  //   std::ranges::set_union(a.parent|std::views::values,b.parent|std::views::values,)
  // }

  explicit operator std::set<std::set<value_t>>()
  {
    std::set<std::set<value_t>> result;
    std::map<value_t,std::set<value_t>> temp;
    for (auto&&x:parent|std::views::keys)    
      temp[find(x)].insert(x);
    for (auto&&[k,v]:temp)
      result.insert(std::move(v));
    // if(result.empty())
    //   result.insert({});
    return result;
  } 
};

// template <std::unsigned_integral T>
// class disjoint_set<T> {
// private:
//   using value_t = T;
//   using size_type = T;
//   std::vector<value_t> parent;
//   std::vector<size_type> rank;

// public:
//   disjoint_set() = default;
//   disjoint_set(size_type size) noexcept : parent(size), rank(size, 1) {
//     std::iota(parent.begin(), parent.end(), 0);
//   }
//   value_t find(value_t x) {
//     return x == parent[x] ? x : (parent[x] = find(parent[x]));
//   }
//   void merge(value_t x, value_t y) {
//     x = find(x);
//     y = find(y);
//     if (rank[x] <= rank[y])
//       parent[x] = y;
//     else
//       parent[y] = x;
//     if (rank[x] == rank[y] && x != y)
//       rank[y]++;
//   }
//   size_type size()const { return parent.size();}
// };

