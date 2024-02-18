#include <ios>
#include <algorithm>
#include <ranges>
#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include "../libs/dbg.h"

class PHCIndex
{
  using size_type = size_t;
  std::vector<std::vector<std::pair<size_type,size_type>>> m_data;
public:
  PHCIndex(const std::filesystem::path &path)
  {
    std::ifstream ifs{path};
    size_type num_vert;
    ifs>>num_vert;
    for(size_type i = 0 ; i < num_vert; i++)
    {
      m_data.emplace_back();      
      size_type num_item;
      ifs>>num_item;
      for (size_type j = 0 ; j < num_item; j++)
      {
        size_type t1,t2;
        ifs>>t1>>t2;
        m_data[i].emplace_back(t1,t2);
      }
    }
    dbg(m_data);
  }
  auto query(size_type ts, size_type te)
  {
    std::vector<size_type> res;
    for(size_type i = 0 ; i < m_data.size(); i++)
      if(query(i,ts,te))
        res.push_back(i);
    return res;
  }
  bool query(size_type v, size_type ts, size_type te)
  {
    auto iter = std::ranges::lower_bound(m_data[v]|std::views::reverse,ts,std::greater{},[](auto interval){return interval.first;}); 
    return iter!=m_data[v].rend() && iter->second<=te;
  }
};

int main()
{
  auto phc = PHCIndex("phc.txt");
  while(true)
  {
    size_t ts,te;
    std::cin>>ts>>te;
    for(auto && v : phc.query(ts,te))
      std::cout<<v<<" ";
    std::cout<<std::endl;
  }
}
