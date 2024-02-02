#include "dbg.h"
#include <algorithm>
#include <concepts>
#include <deque>
#include <functional>
#include <set>
#include <span>
#include <unordered_set>
#include <vector>
#include <chrono>
#include "graph.hpp"

using vert_t = size_t;
using size_type = size_t;

int main() {
  size_t num_vert = 6;
  const auto time_load {std::chrono::steady_clock::now()};
  auto G = load_graph<adjacent_array>("data.txt");
  std::vector<vert_t> P;
  std::vector<vert_t> C;
  std::vector<vert_t> X;
  std::ranges::copy(G.verts(),std::back_inserter(C));
  std::vector<size_type> degree_list(G.verts().size(), 0);
  static_assert(AdjacencyGraph<adjacent_array>);
  const auto time_func {std::chrono::steady_clock::now()};
  bron_kerbosch(G, 4, [](auto x) { dbg("plex found:", x); }, degree_list, P, C, C, {});
  const auto time_test {std::chrono::steady_clock::now()};
  std::cout << "--------------------------------------------------algorithm finished" << std::endl
            << "total rec times: " << rec_time << std::endl
            << "load graph cost: " <<std::chrono::duration<double>{time_func-time_load}.count()<<std::endl
            << "func bronk cost: " <<std::chrono::duration<double>{time_test-time_func}.count()<<std::endl;
  dbg(core_value(G));
}
