#include "../libs/range_tree.hpp"
#include "../libs/rich.hpp"
#include <iterator>
#include <map>
#include <vector>
int main(){
  static_assert(std::ranges::random_access_range<decltype(std::declval<std::vector<std::pair<int,int>>>()|std::views::keys)>);
  // auto data = std::vector{1,2,3,4,5,6,7,8,9};
  // auto data = std::vector{1,2,3,4,5,6,7,8,9};
  // auto tree = binary_tree{data};
  // rich::console.println();
  // tree.traverse();
  using index_type = size_t;
  using point_type = std::tuple<size_t,size_t,size_t>;
  std::map<point_type,float> data;
  data[{5,1,2}] = 1.2;
  data[{7,1,4}] = 2.2;
  data[{1,1,4}] = 3.5;
  data[{1,1,4}] = 4.0;
  data[{8,1,2}] = 5.2;
  data[{8,1,5}] = 5.2;
  data[{9,1,2}] = 1.2;
  auto tree = range_tree<point_type,float>{data};
  // tree.print();
  while(true){
    auto[lower,upper]=rich::console.read<std::pair<point_type,point_type>>("input an interval");
    std::vector<float> temp1;
    // std::vector<float> temp2;
    tree.query(lower,upper,[&](const auto&x){temp1.push_back(x);});
    // tree._query(lower,upper,[&](const auto&x){temp2.push_back(x);});
    rich::console.println("the result 1 is {}",temp1);
    // rich::console.println("the result 2 is {}",temp2);
  }
}
