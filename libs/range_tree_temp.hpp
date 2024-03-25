#include "../libs/rich.hpp"
#include <algorithm>
#include <iterator>
#include <memory>
#include <ranges>
#include <span>
#include <stdexcept>
#include <utility>
#include <vector>

template <typename TPoint, size_t Dimension = std::tuple_size_v<TPoint>>
struct range_tree_node {
  using point_type = TPoint;
  using key_type = std::tuple_element_t<Dimension - 1, TPoint>;
  using size_type = size_t;
  static constexpr key_type at_dim(const point_type &p) noexcept {
    return std::get<Dimension - 1>(p);
  }
  range_tree_node() = default;
  range_tree_node(std::span<point_type> points) {
    if (points.empty())
      throw std::logic_error("build range tree with no point");
    auto unique_indice = std::vector<size_type>{0};
    std::ranges::sort(points, {}, at_dim);
    auto prev = at_dim(points[0]);
    for (size_type i = 0; i < points.size(); prev = at_dim(points[i]), i++) {
      if (at_dim(points[i]) != prev)
        unique_indice.push_back(i);
    }
    build_tree(points, unique_indice, 0);
  }
  range_tree_node(std::span<point_type> points, std::span<size_type> unique_indice, size_type base_index = 0) {
    build_tree(points, unique_indice, base_index);
  }
  void build_tree(std::span<point_type> points, std::span<size_type> unique_indice, size_type base_index) {
    if (unique_indice.empty())
      throw std::logic_error("build range tree with no point");
    auto mid = (unique_indice.size() - 1) / 2;
    _key = at_dim(points[unique_indice[mid] - base_index]);
    if (unique_indice.size() == 1) {
      next_dim = std::make_shared<range_tree_node<TPoint, Dimension - 1>>(points);
    } else {
      auto lower_indice = unique_indice.subspan(0, mid + 1);
      auto upper_indice = unique_indice.subspan(mid + 1);
      auto mid_base = unique_indice[mid + 1] - base_index;
      if (!lower_indice.empty()) {
        lower_child = std::make_shared<range_tree_node<TPoint, Dimension>>(points.subspan(0, mid_base), lower_indice, base_index);
      }
      if (!upper_indice.empty()) {
        upper_child = std::make_shared<range_tree_node<TPoint, Dimension>>(points.subspan(mid_base), upper_indice, mid_base);
      }
      next_dim = std::make_shared<range_tree_node<TPoint, Dimension - 1>>(lower_child->next_dim, upper_child->next_dim);
    }
  }
  template <typename Fn>
  void query(const point_type &lower, const point_type &upper, Fn &&func) {
    rich::console.println("query key {}", _key);
    auto lower_key = std::get<Dimension - 1>(lower);
    auto upper_key = std::get<Dimension - 1>(upper);
    if (is_leaf()) {
      if (lower_key <= _key && _key <= upper_key)
        return next_dim->query(lower, upper, func);
    }
    if (upper_key < _key) {
      if (lower_child)
        lower_child->query(lower, upper, func);
    } else if (lower_key > _key) {
      if (upper_child)
        upper_child->query(lower, upper, func);
    } else {
      auto lower_cur = lower_child;
      while (lower_cur) {
        if (lower_cur->is_leaf()) {
          if (lower_key <= lower_cur->_key)
            lower_cur->next_dim->query(lower, upper, func);
          break;
        } else if (lower_key <= lower_cur->_key) {
          if (lower_cur->upper_child)
            lower_cur->upper_child->next_dim->query(lower, upper, func);
          lower_cur = lower_cur->lower_child;
        } else
          lower_cur = lower_cur->upper_child;
      }
      auto upper_cur = upper_child;
      while (upper_cur) {
        if (upper_cur->is_leaf()) {
          if (upper_key >= upper_cur->_key)
            upper_cur->next_dim->query(lower, upper, func);
          break;
        } else if (upper_key > upper_cur->_key) {
          if (upper_cur->lower_child)
            upper_cur->lower_child->next_dim->query(lower, upper, func);
          upper_cur = upper_cur->upper_child;
        } else
          upper_cur = upper_cur->lower_child;
      }
    }
  }
  [[nodiscard]] constexpr bool is_leaf() const noexcept { return !lower_child && !upper_child; }
  key_type _key;
  std::shared_ptr<range_tree_node<TPoint, Dimension>> lower_child;
  std::shared_ptr<range_tree_node<TPoint, Dimension>> upper_child;
  std::shared_ptr<range_tree_node<TPoint, Dimension - 1>> next_dim;
};

template <typename TPoint>
struct range_tree_node<TPoint, 1> {
  using point_type = TPoint;
  using key_type = std::tuple_element_t<0, TPoint>;
  using size_type = size_t;
  range_tree_node() = default;
  // range_tree_node(std::span<point_type> points, std::span<size_type> unique_indice){
  //   if(points.empty()) throw std::logic_error("build range tree with no point");
  //   std::ranges::copy(unique_indice|std::views::transform([&](auto i){return points[i-1];}));
  // }
  range_tree_node(std::span<point_type> points) {
    std::ranges::sort(points, {}, [](const auto &x) { return std::get<0>(x); });
    std::ranges::copy(points | std::views::elements<0>, std::back_inserter(_data));
  }
  range_tree_node(const std::shared_ptr<range_tree_node> &lower_child, const std::shared_ptr<range_tree_node> &upper_child) {
    std::merge(lower_child->_data.begin(), lower_child->_data.end(), upper_child->_data.begin(), upper_child->_data.end(), std::back_inserter(_data));
  }
  template <typename Fn>
  void query(const point_type &lower, const point_type &upper, Fn &&func) {
    auto lower_iter = std::ranges::lower_bound(_data, std::get<0>(lower));
    auto upper_iter = std::ranges::upper_bound(_data, std::get<0>(upper));
    std::ranges::for_each(lower_iter, upper_iter, func);
  }
  std::vector<key_type> _data;
};

template <typename TPoint>
struct range_tree {
  using point_type = TPoint;
  using size_type = size_t;
  constexpr static size_type Dimension = std::tuple_size_v<TPoint>;
  range_tree(std::span<point_type> points) {
    auto unique_indice = std::vector<size_type>{};
    std::ranges::sort(points, {}, [](const auto &x) { return std::get<Dimension - 1>(x); });
    auto prev = std::get<Dimension - 1>(points[0]);
    unique_indice.push_back(0);
    for (size_type i = 0; i < points.size(); i++) {
      if (std::get<Dimension - 1>(points[i]) != prev)
        unique_indice.push_back(i);
      prev = std::get<Dimension - 1>(points[i]);
    }
    // unique_indice.push_back(points.size());
    _root = decltype(_root){points, std::span{unique_indice}};
  }
  template <typename Fn>
  void query(const point_type &lower, const point_type &upper, Fn &&func) {
    _root.query(lower, upper, func);
  }
  range_tree_node<TPoint> _root;
};
