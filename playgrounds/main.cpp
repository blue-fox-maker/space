#include "../libs/dbg.h"
#include "../libs/disjoint_set.hpp"
#include "../libs/load.hpp"
#include "../libs/pprint.hpp"
#include "../libs/rich.hpp"
#include "../libs/temporal_index.hpp"
#include <algorithm>
#include <format>
#include <fstream>
#include <iterator>
#include <map>
#include <ranges>
#include <thread>
#include <vector>

int main() {

  pprint::PrettyPrinter printer;
  auto console = rich::console{};
  console.rule("load data");
  auto [num_vert, num_edge, num_time, partitions] = load<std::tuple<size_t, size_t, size_t, std::map<std::pair<size_t, size_t>, std::set<size_t>>>>("data/data_core.txt");
  auto temporal_communites = std::map<std::tuple<size_t, size_t>, std::set<std::set<size_t>>>{};
  for (auto &&[k, v] : partitions)
    temporal_communites[k].insert(v);
  console.para("load data finished");
  console.para(std::format("load graph with {} verts and {} edges in [0,{})", num_vert, num_edge, num_time));
  console.rule("construct lne index");
  auto lne = LneIndex{num_vert, num_time, temporal_communites};
  console.para(std::format("construct lne finished of size {}", lne.size()));
  // printer.print(lne.data());
  auto phc = PhcIndex{num_vert, num_time, [&](size_t ts, size_t te, size_t v) { return partitions[{ts, te}].contains(v); }};
  console.para(std::format("Construct phc finished of size {}", phc.size()));
  // printer.print(phc.data());
  console.rule("test");
  auto errors = std::vector<std::tuple<size_t, size_t, std::set<std::set<size_t>>, std::set<std::set<size_t>>>>{};
  for (auto ts : std::views::iota(0U, num_time) | rich::views::track()) {
    for (auto te : std::views::iota(ts, num_time)) {
      if (!partitions[{ts,te}].empty() && (std::set<std::set<size_t>>)lne.query(ts, te) != temporal_communites[{ts, te}] ) {
        errors.push_back({ts,te,(std::set<std::set<size_t>>)lne.query(ts, te), temporal_communites[{ts,te}]});
      }
    }
  }
  console.para("test finished");
  for (auto &&[ts, te, res1, res2] : errors) {
    console.para(std::format("Error in [{}, {}]", ts, te));
    console.para("the expected result is:");
    printer.print(res1);
    console.para("but get:");
    printer.print(res2);
  }

  // while (true) {
  //   size_t ts, te;
  //   std::cin >> ts >> te;
  //   auto res = lne.query(ts, te);
  //   auto phc_res = phc.query(ts,te);
  //   printer.print((std::set<std::set<size_t>>)res);
  //   printer.print(phc_res);
  //   printer.print((std::set<std::set<size_t>>)disjoint_set{temporal_communites[{ts,te}]});
  //   rich::console::para(std::format("test in interval [{},{}]",ts,te));
  // }
}
