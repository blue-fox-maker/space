#include "../libs/dbg.h"
#include "../libs/range_tree.hpp"
// #include "../libs/layered_range_tree.hpp"
#include "../libs/rich.hpp"
#include <cassert>
#include <iterator>
#include <random>
#include <ranges>

int main() {
  std::default_random_engine rd;
  std::uniform_int_distribution<size_t> dist(0, 50);
  std::vector<RangeTree::Point<size_t, size_t>> data;
  for (auto i : rich::console.track(std::views::iota(0, 50)))
    data.push_back({{dist(rd), dist(rd), dist(rd)}, dist(rd)});

  RangeTree::RangeTree<size_t, size_t> tree{data};
  rich::console.rule("query data");
  for (auto i : rich::console.track(std::views::iota(0, 50))) {
    auto l1 = dist(rd);
    auto l2 = dist(rd);
    auto l3 = dist(rd);
    auto u1 = dist(rd);
    auto u2 = dist(rd);
    auto u3 = dist(rd);
    std::array<size_t, 3> lower = {l1, l2, l3};
    std::array<size_t, 3> upper = {u1, u2, u3};
    // auto result = tree.pointsInRange({l1, l2, l3}, {u1, u2, u3}, {true, true, true}, {true, true, true});
    auto result = tree.pointsInRange({l1, l2, l3}, {u1, u2, u3});
    std::set<RangeTree::Point<size_t, size_t>> temp{result.begin(), result.end()};
    for (auto point : data) {
      auto p1 = point.pos()[0];
      auto p2 = point.pos()[1];
      auto p3 = point.pos()[2];
      if (l1 <= p1 && l2 <= p2 && l3 <= p3 && p1 <= u1 && p2 <= u2 && p3 <= u3) {
          // assert((temp.contains(point)));
        if(!temp.contains(point)){
          rich::console.para(std::format("in query range [{}, {}, {}] [{}, {}, {}]",l1,l2,l3,u1,u2,u3));
          rich::console.show(point.pos());
        }
      }
    }
  }
  // auto result = tree.pointsInRange({0, 4}, {8, 5}, {true, true}, {true, true});
  // rich::console.show(result | std::views::transform([](const auto &x) { return x.value(); }));
}
