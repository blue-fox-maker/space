#include "temporal.hpp"
#include <fstream>
#include <map>
#include "../libs/dbg.h"

int main()
{
  std::ifstream ifs{"core.txt"};
  std::map<std::pair<size_t,size_t>,std::set<size_t>> core;
  size_t n;  
  ifs>>n;
  for(size_t i = 0 ; i < n ; i ++ )
  {
    size_t ts,te,m;
    ifs>>ts>>te>>m;
    if(m==0) continue;
    core[{ts,te}] = {};
    for(size_t j = 0 ; j < m ; j++)
    {
      size_t v;
      ifs>>v;
      core[{ts,te}].insert(v);
    }
  }
  dbg(core);
  auto is_equivalent = [&](auto ts, auto te, auto u, auto v){
    return core.contains({ts,te}) && core[{ts,te}].contains(u) && core[{ts,te}].contains(v);
  };
  auto lne = LneIndex{13,8,is_equivalent};
  std::cout<<"hello world"<<std::endl;
}
