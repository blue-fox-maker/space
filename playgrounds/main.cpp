#include "../libs/dbg.h"
#include "../libs/disjoint_set.hpp"
#include "../libs/load.hpp"
#include "../libs/rich.hpp"
#include "../libs/temporal_index.hpp"
#include "../libs/range_tree.hpp"
#include <random>
#include <algorithm>
#include <format>
#include <fstream>
#include <iterator>
#include <map>
#include <ranges>
#include <thread>
#include <vector>
#include <chrono>

int main() {

  auto console = rich::console{};

  console.rule("load data");
  // auto [num_vert, num_edge, num_time,temporal_communites] = console.load<std::tuple<size_t, size_t, size_t, std::map<std::pair<size_t, size_t>, std::set<std::set<size_t>>>>>("data/data_cc.txt");
  auto [num_vert, num_edge, num_time,partitions] = console.load<std::tuple<size_t, size_t, size_t, std::map<std::pair<size_t, size_t>, std::set<size_t>>>>("data/data_core.txt");
  console.para(std::format("load graph with {} verts and {} edges in [0,{})", num_vert, num_edge, num_time));

  // auto temporal_communities = partitions;
  auto temporal_communities = std::map<std::pair<size_t, size_t>, std::set<std::set<size_t>>>{};
  for (auto &&[k, v] : partitions)
    if (!v.empty())
      temporal_communities[k].insert(v);

  console.para("load data finished");

  console.rule("construct lne index");
  auto lne = LneIndex{num_vert, num_time, temporal_communities};
  console.para(std::format("construct lne finished of size {}", lne.size()));


  // console.rule("test");
  // auto errors = std::vector<std::tuple<size_t, size_t, std::set<std::set<size_t>>, std::set<std::set<size_t>>>>{};
  // for (auto ts : console.track(std::views::iota(0U, num_time))) {
  //   for (auto te : std::views::iota(ts, num_time)) {
  //     if ((std::set<std::set<size_t>>)lne.query(ts, te) != temporal_communities[{ts, te}]) {
  //       errors.push_back({ts,te,(std::set<std::set<size_t>>)lne.query(ts, te), temporal_communities[{ts,te}]});
  //     }
  //   }
  // }
  // console.para("test finished");
  // for (auto &&[ts, te, res1, res2] : errors) {
  //   console.para(std::format("error in [{}, {}]", ts, te));
  //   console.para("the expected result is:");
  //   console.show(res2);
  //   console.para("but get:");
  //   console.show(res1);
  // }

  auto phc= PhcIndex(num_vert,num_time,[&](auto ts, auto te, auto v){return partitions[{ts,te}].contains(v);});
  console.para(std::format("the size of phc is {}",phc.size()));

  std::random_device rd;
  std::uniform_int_distribution<size_t> distrib(0,num_time-1);  
  size_t num_test = 1000000;
  std::vector<std::pair<size_t,size_t>> test_args;
  for(auto i: std::views::iota(0U,num_test))
    test_args.push_back(std::minmax(distrib(rd),distrib(rd)));

  console.rule("test lne efficiency");
  auto phc_time = std::chrono::system_clock::now();
  for (auto [ts,te]: test_args)
    phc.query(ts,te);
  console.para(std::format("phc finished {} test cases in {:%T}", num_test, std::chrono::system_clock::now()-phc_time));
  auto lne_time = std::chrono::system_clock::now();
  for (auto [ts,te]: test_args)
    lne.query(ts,te);
  console.para(std::format("lne finished {} test cases in {:%T}", num_test, std::chrono::system_clock::now()-lne_time));
  
  // while (true) {
  //   size_t ts, te;
  //   std::cin >> ts >> te;
  //   auto res = lne.query(ts, te);
  //   console.show(temporal_communities[{ts,te}]);
  //   console.show((std::set<std::set<size_t>>)res);
  //   console.show(phc.query(ts,te));
  //   console.para(std::format("test in interval [{},{}]",ts,te));
  // }
}
