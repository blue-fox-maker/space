#include "../libs/disjoint_set.hpp"
#include "../libs/index.hpp"
#include "../libs/rich.hpp"
#include <cassert>
#include <iterator>
#include <random>
#include <ranges>
#include <map>
#include <set>
#include <iostream>

int main() {
  auto [num_vert, num_edge, num_time, data] = rich::console.load<std::tuple<size_t, size_t, size_t, std::map<std::pair<size_t, size_t>, std::set<std::set<size_t>>>>>(std::filesystem::path("data/data_cc.txt"));
  // auto [num_vert, num_edge, num_time, data] = rich::console.load<std::tuple<size_t, size_t, size_t, std::map<std::pair<size_t, size_t>, std::set<size_t>>>>(std::filesystem::path("data/data_core.txt"));
  auto temporal_communities = std::map<std::pair<size_t,size_t>,disjoint_set<size_t>>{};
  for (auto &&[k,communites]:data){
    temporal_communities[k] = communites;
  }
  auto lne_index = temporal_index{num_vert,num_time,temporal_communities};
  for (auto ts: std::views::iota(0U,num_time)){
    for (auto te: std::views::iota(ts,num_time)){
      if ((std::set<std::set<size_t>>)temporal_communities[{ts,te}] != (std::set<std::set<size_t>>)lne_index.query(ts,te)){
        // std::cout<<"error"<<ts<<" "<<te<<std::endl;
        rich::console.println("error in query in [{},{}]",ts,te);
        rich::console.println("the expected result is: {}", (std::set<std::set<size_t>>)temporal_communities[{ts,te}]);
        rich::console.println("but got: {}", (std::set<std::set<size_t>>)lne_index.query(ts,te));
      }
    }
  }
  rich::console.println("# TEST FINISHED");
}
