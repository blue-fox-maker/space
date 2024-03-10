#include <vector>
#include <algorithm>
#include <ranges>
#include <map>

template <size_t Dimension>
class range_tree_node{
  using size_type = typename range_tree_node<Dimension-1>::size_type;
  using key_type = std::array<size_type,Dimension>;
  template <std::output_iterator<size_type> I>
  void query(const key_type& lower, const key_type& upper, I iter){
    const auto lower_key = std::get<0>(lower);
    const auto upper_key = std::get<0>(upper);
    // the range tree query something
  }
};

template <>
class range_tree_node<1>{
  using size_type = size_t;
  using key_type = std::array<size_type,1>;
  template <std::output_iterator<size_type> I>
  void query(const key_type& lower,const key_type& upper ,I iter){
    std::ranges::copy(std::ranges::lower_bound(data,std::get<0>(lower)), std::ranges::upper_bound(data,std::get<0>(upper)),iter);
  }
  
private:
  std::vector<size_type> data;
};

template <typename T, size_t Dimension> 
class range_tree {
public:
  using node_type = range_tree_node<Dimension>;
  using size_type = typename node_type::size_type;
  using mapped_type = T;
  using key_type = std::array<size_type,Dimension>;

  range_tree(const std::map<key_type,mapped_type> &values):data(values.size()){
    std::ranges::copy(values|std::views::values,data.begin());
    std::vector<key_type> temp(values.size());
    std::ranges::copy(values|std::views::keys,temp.begin());
    root = temp;
  }
private:
  std::vector<mapped_type> data;
  range_tree_node<Dimension> root; //? really
};

