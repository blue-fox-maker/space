#pragma once
#include "graph.hpp"
#include <concepts>
#include <iterator>
#include <ranges>
#include <span>
#include <vector>

template <typename T>
std::ostream &operator<<(std::ostream &os, const list<T> &value) {
  return os << "{ " << value.active_elements() << "| " << value.inactive_elements() << "}";
}

template <typename T>
std::ostream &operator<<(std::ostream &os, const std::span<T> &value) {
  if (value.empty())
    return os;
  else if (value.size() == 1)
    return os << value.front();
  else
    return os << value.front() << ", " << value.subspan(1);
}

template <std::ranges::forward_range T>
std::ostream &operator<<(std::ostream &os, const T &value) {
  os << "{ " << std::span(value) << " }";
}
