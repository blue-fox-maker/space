#include <functional>
#include <cassert>
#include <span>
#include <algorithm>
#include <type_traits>
#include <vector>
#include <ranges>
#include "../libs/index.hpp"
#include "../libs/rich.hpp"

template <typename Id, typename Stamp>
struct temporal_index{
  using index_type = Id;

  index_type cur_x = 0;
  index_type cur_y = 0;
  index_type num_vert;

  std::vector<disjoint_set<id_t>> ccs; 
  std::vector<std::tuple<index_type,index_type,index_type,index_type,index_type>> index;
  temporal_index(index_type num_vert):num_vert(num_vert){}
  void update(index_type u, index_type v, index_type ta, index_type tb){
    assert(ta<=cur_x||tb>cur_y);
    cur_x = ta;
    cur_y = tb;
    while(ccs.size()<=cur_x) ccs.emplace_back(num_vert);
    if(!ccs[cur_x].is_eq(u,v)){
      index_type t = 0;
      for(size_t i=cur_x; i!=0; i--){
        if(!ccs[i].merge(u,v)){
          t = i+1;
          break;
        }
      } 
      index.emplace_back(u,v,t,ta,tb);
    }
  }
  auto query(index_type lower, index_type upper){
    auto etf = eular_tour_forest<index_type>{num_vert};
    for(auto [u,v,t,ta,tb]: index){
      if(t<=lower&&lower<=ta&&tb<=upper){
        etf.merge(u,v);
      }
    }
    auto is_visited = std::vector<bool>(num_vert,false);
    for(auto i:std::views::iota(0U,num_vert))
    {
      if(!is_visited[i])
      {
        auto temp = std::vector<index_type>{};
        auto v = i;
        do{
          v = etf.next(v);
          temp.push_back(v);
          is_visited[v] = true;
        }while(v!=i);
        std::ranges::sort(temp);
        rich::console.println("CC found: {}", temp);
      }
    }
  }
  constexpr index_type size() const noexcept { return index.size();}
};
