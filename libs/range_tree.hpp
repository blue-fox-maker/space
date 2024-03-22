#include <algorithm>
#include <iostream>
#include <memory>
#include <ranges>
#include <stdexcept>
#include <tuple>
#include <vector>

template <typename TPoint, typename TValue, size_t Dimension = std::tuple_size_v<TPoint>>
class range_tree {
public:
  using point_type = TPoint;
  using value_type = TValue;
  using size_type = size_t;
  using key_type = std::tuple_element_t<Dimension - 1, TPoint>;

  range_tree(std::span<point_type> points) {
    if (points.empty()) throw std::logic_error("construct range tree with no point");
    std::ranges::sort(points, {}, [](const point_type &x) { return std::get<Dimension - 1>(x); });
    auto mid = (points.size()-1) / 2;
    // std::cout<<mid<<" ";
    _key = std::get<Dimension - 1>(points[mid]);
    if(points.size()!=1){
      auto lower_span = points.subspan(0, (points.size() + 1) / 2);
      if (!lower_span.empty())
        lower_child = std::make_shared<range_tree<TPoint,TValue,Dimension>>(lower_span);
      auto upper_span = points.subspan((points.size() + 1) / 2);
      if (!upper_span.empty())
        upper_child = std::make_shared<range_tree<TPoint,TValue,Dimension>>(upper_span);
    }
    next_dim = std::make_shared<range_tree<TPoint,TValue,Dimension-1>>(points);
  }
  template <std::output_iterator<key_type> Iter>
  void query(const point_type &lower, const point_type &upper, Iter result) {
    auto lower_key = std::get<Dimension - 1>(lower);
    auto upper_key = std::get<Dimension - 1>(upper);
    if(is_leaf()){
      if (lower_key<=_key &&_key<=upper_key)
        next_dim->query(lower,upper,result);
    } else if (upper_key <= _key) {
      if(lower_child)
        lower_child->query(lower, upper, result);
    } else if (lower_key > _key) {
      if(upper_child)
        upper_child->query(lower, upper,result);
    } else {
      auto lower_cur = lower_child;
      while(lower_cur){
        if (!lower_cur->lower_child&&!lower_cur->upper_child){
          if(lower_key<=lower_cur->_key) lower_cur->next_dim->query(lower,upper,result);
          break;          
        }
        else if(lower_key<=lower_cur->_key){
          if(lower_cur->upper_child)
            lower_cur->upper_child->next_dim->query(lower,upper,result);
          lower_cur = lower_cur->lower_child;
        } else lower_cur = lower_cur->upper_child;
      }
      auto upper_cur = upper_child;
      while(upper_cur){
        if (!upper_cur->lower_child&&!upper_cur->upper_child){
          if(upper_key>=upper_cur->_key) upper_cur->next_dim->query(lower,upper,result);
          break;          
        }
        else if(upper_key>upper_cur->_key){
          if(upper_cur->lower_child)
            upper_cur->lower_child->next_dim->query(lower,upper,result);
          upper_cur = upper_cur->upper_child;
        } else upper_cur = upper_cur->lower_child;
      }
    }
  }

private:
  bool is_leaf(){return !lower_child&&!upper_child;}
  std::shared_ptr<range_tree<TPoint, TValue, Dimension>> lower_child;
  std::shared_ptr<range_tree<TPoint, TValue, Dimension>> upper_child;
  std::shared_ptr<range_tree<TPoint, TValue, Dimension - 1>> next_dim;
  key_type _key;
};

template <typename TPoint, typename TValue>
class range_tree<TPoint,TValue ,1> {
public:
  using point_type = TPoint;
  using value_type = TValue;
  using size_type = size_t;
  using key_type = std::tuple_element_t<0, TPoint>;

  range_tree(std::span<point_type> points) {
    if (points.empty())
      throw std::logic_error("construct range tree with no point");
    std::ranges::sort(points, {}, [](const point_type &x) { return std::get<0>(x); });
    std::ranges::copy(points | std::views::elements<0>, std::back_inserter(_data));
  }
  template <std::output_iterator<key_type> Iter>
  void query(const point_type &lower, const point_type &upper, Iter result) {
    auto lower_iter = std::ranges::lower_bound(_data, std::get<0>(lower));
    auto upper_iter = std::ranges::upper_bound(_data, std::get<0>(upper));
    std::copy(lower_iter, upper_iter, result);
  }

private:
  std::vector<key_type> _data;
};
