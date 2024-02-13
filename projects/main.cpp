#include "../libs/dbg.h"
#include "../libs/graph.hpp"
#include "../libs/load.hpp"
#include "../libs/temporal_index.hpp"
#include <algorithm>
#include <fstream>
#include <iterator>
#include <map>
#include <ranges>
#include <vector>

int main() {
  size_t k = 3;
  using index_t = size_t;
  std::ifstream ifs{"data/data_core.txt"};
  std::map<std::pair<size_t, size_t>, std::set<size_t>> temporal_core;
  size_t num_vert, num_edge, num_time;
  load(std::istream_iterator<size_t>{ifs}) >> num_vert >> num_edge >> num_time >> temporal_core;
  auto is_equivalent = [&](size_t ts, size_t te, size_t u, size_t v) {
    auto &core = temporal_core[{ts, te}];
    return core.contains(u) && core.contains(v);
  };
  dbg("Load data finished");
  auto lne = LneIndex{num_vert, num_time, is_equivalent};
  dbg("Construct phc finished of size", lne.size());
  auto phc = PhcIndex{num_vert, num_time, [&](size_t ts, size_t te, size_t v) { return temporal_core[{ts, te}].contains(v); }};
  dbg("Construct lne finished of size", phc.size());
  while (true) {
    index_t ts, te;
    std::cin >> ts >> te;
    auto res = lne.query(ts, te);
    auto display_res = std::map<size_t, std::vector<size_t>>{};
    auto lne_res = std::set<size_t>{};
    auto phc_res = std::set<size_t>{};
    for (size_t i = 0; i < num_vert; i++)
      display_res[res.find(i)].push_back(i);
    for (auto cls : display_res | std::views::values)
      if (cls.size() > 1)
        std::ranges::copy(cls,std::inserter(lne_res,lne_res.begin()));
    phc_res = phc.query(ts, te);
    dbg(lne_res);
    dbg(phc_res);
    auto &truth = temporal_core[{ts, te}];
    dbg(lne_res == truth);
    dbg(phc_res == truth);
  }
}
