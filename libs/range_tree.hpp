#include <vector>
#include <algorithm>
#include <ranges>
#include <map>
#include "../libs/rich.hpp"

template <typename T>
struct tree_view{
  using container = std::span<T>; 
  using element_type = typename container::element_type;
  using value_type = typename container::value_type;
  using size_type = typename container::size_type;
  using difference_type = typename container::difference_type;
  using reference = typename container::reference;
  using self_type = tree_view<T>;
  [[nodiscard]] reference root() const noexcept { return c[c.size()/2];}
  [[nodiscard]] constexpr self_type lower_child() const noexcept { return c.subspan(0,c.size()/2);}
  [[nodiscard]] constexpr self_type upper_child() const noexcept { return c.subspan(c.size()/2+1);}
  [[nodiscard]] constexpr bool empty() const noexcept { return c.empty();} 
  [[nodiscard]] constexpr bool has_lower_child() const noexcept { return c.size()>=2;}
  [[nodiscard]] constexpr bool has_upper_child() const noexcept { return c.size()>=3;}
  [[nodiscard]] constexpr size_type size() const noexcept { return c.size();}
  tree_view(container c):c(c){}
  
  container c;
};

template <typename TPoint, size_t Dimension = std::tuple_size_v<TPoint>>
class range_tree_node{
public:
  using point_type = TPoint;
  using key_type = std::tuple_element_t<Dimension-1,point_type>;
  using node_type = range_tree_node<TPoint,Dimension-1>;
  key_type key;
  range_tree_node() = default;
  range_tree_node(std::span<point_type> points):nodes(2*points.size()-1){
    rich::console.println("init node of dim {} with points {}", Dimension, points);
    std::ranges::sort(points,{},[](const auto&x){return std::get<Dimension-1>(x);});
    key = std::get<Dimension-1>(points[points.size()/2]);
    build_tree(tree_view<node_type>{std::span<node_type>{nodes}},points);
  }
  void build_tree(tree_view<node_type> tree, std::span<point_type> points){
    rich::console.println("size of tree {} and of points {}", tree.size(), points.size());
    if(points.size()==0) return;
    auto mid = (points.size()-1)/2;
    if(!tree.empty())
      tree.root() = node_type{points};
    if(tree.has_lower_child())
      build_tree(tree.lower_child(), points.subspan(0,mid+1));
    if(tree.has_upper_child())
      build_tree(tree.upper_child(), points.subspan(mid+1));
  }
  auto query(const point_type& lower, const point_type upper){
    auto tree = tree_view<node_type>{std::span<node_type>{nodes}};
    while(tree.root().key)
  }
  const auto& data()const { return nodes;}
private:
  std::vector<node_type> nodes;
};

template <typename TPoint>
class range_tree_node<TPoint,0>{
public:
  range_tree_node() = default;
  using point_type = TPoint;
  using key_type = std::tuple_element_t<0,point_type>;
  range_tree_node(std::span<point_type> points){
    key = std::get<0>(points[(points.size()-1)/2]);
  }
  key_type key;
};

template <typename TPoint, typename TVal>
class range_tree{
public:
  using value_type = TVal;
  using point_type = TPoint;
  using size_type = size_t;
  template <std::ranges::input_range R>
  explicit range_tree(R&&rng){
    std::ranges::copy(rng|std::views::values,std::back_inserter(values));
    std::vector<point_type> points(values.size());
    std::ranges::copy(rng|std::views::keys,points.begin());
    rich::console.println("the points are {}",points);
    root = range_tree_node<point_type>{points};
  }
  const auto data() const { return root.data();}
  auto query(const point_type &lower, const point_type &upper ){
    root.query(lower,upper);
  }
private:
  std::vector<value_type> values;
  range_tree_node<point_type> root;
};
