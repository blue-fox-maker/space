#include "../libs/graph.hpp"

int main()
{
  static_assert(graph::edge_range<std::vector<graph::edge<int,void>>>);  
  // static_assert(graph::edge<std::tuple<int,int,int>>);  
}
