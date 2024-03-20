// #include "../libs/disjoint_set.hpp"
#include "../libs/graph.hpp"
#include "../libs/rich.hpp"
#include <fstream>
#include <ranges>
#include <tuple>

int main(){
  using value_t = size_t;
  using stamp_t = size_t;
  auto ifs = std::ifstream{"data/graph.txt"};
  auto [num_vert,edges] = rich::io::load<std::pair<size_t,std::vector<std::tuple<value_t,value_t,stamp_t>>>>(ifs).value();
  auto index = temporal_index<value_t,stamp_t>{num_vert};
  // std::ranges::sort(edges,{},[](const auto&edge){
  //   auto [u,v,t] = edge;
  //   return std::make_pair(edge.upper,edge.upper-edge.lower);});
  rich::console.bench([&](){std::ranges::for_each(edges,[&](const auto&x){
    auto [u,v,t] = x;
    index.update(u,v,t,t);
  });});
  rich::console.println("generate temporal spanning forest of size {}",index.size());
  while(true){
    auto [ts,te] = rich::console.read<std::pair<stamp_t,stamp_t>>("input an interval");
    index.query(ts,te);
  }
}


