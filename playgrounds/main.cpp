// #include "../libs/disjoint_set.hpp"
#include "../libs/graph.hpp"
#include "../libs/rich.hpp"
#include <fstream>
#include <ranges>
#include <tuple>

int main(){
  using index_type = size_t;
  using value_t = size_t;
  using stamp_t = size_t;
  auto ifs = std::ifstream{"data/graph.txt"};
  auto [num_vert,num_time, edges] = rich::io::load<std::tuple<size_t,size_t,std::vector<std::tuple<value_t,value_t,stamp_t>>>>(ifs).value();
  auto index = temporal_index<value_t,stamp_t>{num_vert};
  // std::ranges::sort(edges,{},[](const auto&edge){
  //   auto [u,v,t] = edge;
  //   return std::make_pair(edge.upper,edge.upper-edge.lower);});
  rich::console.bench([&](){std::ranges::for_each(edges,[&](const auto&x){
    auto [u,v,t] = x;
    index.update(u,v,t,t);
  });});
  // std::ranges::sort(index.index,{},[](const auto&x){return std::get<2>(x);});
  rich::console.println("generate temporal spanning forest of size {}",index.size());
  auto query_ifs = std::ifstream{"data/query.txt"};
  auto output_ofs = std::ofstream{"data/output.txt"};
  auto queries = rich::io::load<std::vector<std::pair<size_t,size_t>>>(query_ifs).value();
  //! i build it here
  index.build();
  rich::console.bench([&]{
    for (auto [ts,te]: queries){
      auto etf = index.query(ts,te);
      auto is_visited = std::vector<bool>(num_vert,false);
      output_ofs << std::format("The spanned connected components in [{}, {}] are:\n",ts,te);
      for(auto i:std::views::iota(0U,num_vert))
      {
        if(!is_visited[i])
        {
          auto temp = std::vector<index_type>{};
          auto v = i;
          do{
            v = etf.next(v);
            temp.push_back(v);
            is_visited[v] = true;
          } while(v!=i);
          std::ranges::sort(temp);
          output_ofs << "{ ";
          for(auto x:temp) output_ofs<<x<<" ";
          output_ofs << "}\n";
        }
      }
      output_ofs<<"\n";
    }
  });
}


