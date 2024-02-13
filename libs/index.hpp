#include <concepts>
#include <map>
#include <vector>

template <typename T, std::unsigned_integral TIndex>
class index{
  using index_t = TIndex;
  using value_t = T;
  std::map<value_t,index_t> m_value_to_index;
  std::vector<value_t> m_index_to_value;
public:
  const index_t operator[](const value_t &val)
  {
    auto iter = m_value_to_index.find(val);
    if(iter==m_value_to_index.end())
    {
      index_t ind = m_index_to_value.size();
      m_index_to_value.push_back(val);
      m_value_to_index[val]=ind;
      return ind;
    }
    else return iter->second;
  }
  const value_t &operator[](const index_t ind)
  {
    return m_index_to_value[ind];
  }
  const size_t size() { return m_index_to_value.size();}
};

