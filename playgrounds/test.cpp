#include "../libs/range_tree.hpp"
#include "../libs/rich.hpp"
#include <iterator>
int main(){
  using index_type = size_t;
  using point_type = std::tuple<size_t,size_t>;
  std::vector<std::tuple<size_t,size_t>> points = {
    {5,3},{7,6},{1,2}
  };
  auto tree = range_tree(std::span{points});
  while(true){
    auto[lower,upper]=rich::console.read<std::pair<point_type,point_type>>("input an interval");
    std::vector<index_type> temp;
    tree.query(lower,upper,std::back_inserter(temp));
    rich::console.println("the result is {}",temp);
  }
}
