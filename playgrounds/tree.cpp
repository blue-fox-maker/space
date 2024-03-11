#include "../libs/range_tree.hpp"
#include "../libs/rich.hpp"
#include <map>


int main()
{
  // std::map<std::array<int,3>,float> data = {
  //   {{0,10,20},1.1},
  //   {{1,11,21},6.2},
  //   {{2,12,22},3.3},
  //   {{3,13,23},7.4},
  //   {{4,14,24},7.5},
  //   {{5,15,25},9.6},
  //   {{6,16,26},7.5},
  //   {{7,17,27},9.6},
  // };
  // auto tree = range_tree<std::array<int,3>,float>{data};
  std::map<std::array<int,1>,float> data = {
    {{0},0.12345},
    {{1},1.12345},
    {{2},2.12345},
    {{3},3.12345},
    {{4},4.12345},
    {{5},5.12345},
    {{6},6.12345},
    {{7},7.12345},
  };
  auto tree = range_tree<std::array<int,1>,float>{data};
  for(auto&& node: tree.data())
    std::cout<<" "<< node.key;
}
