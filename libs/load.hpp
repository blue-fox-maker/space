#include <iterator>
#include <source_location>
#include <fstream>
#include <algorithm>
#include <concepts>
#include <filesystem>
#include <map>
#include <ranges>
#include <set>
#include <tuple>
#include <vector>
#include <optional>
#include "rich.hpp"

template <typename TIter>
class loader {
  TIter iter;
  using self_type = loader<TIter>;

public:
  loader(const TIter &iter) : iter(iter){};
  template <typename T>
  self_type &operator>>(T &data) {
    data = *iter++;
    return *this;
  }
  template <typename T1, typename T2>
  self_type &operator>>(std::pair<T1, T2> &data) {
    return *this >> data.first >> data.second;
  }
  template <typename... TArgs>
  self_type &operator>>(std::tuple<TArgs...> &data) {
    std::apply([&](auto &...args){(*this>>...>>args);},data);
    return *this;
  }
  template <typename... TArgs>
  self_type &operator>>(std::vector<TArgs...> &data) {
    size_t num;
    *this >> num;
    data.resize(num);
    for (size_t i = 0; i < num; i++)
      *this >> data[i];
    return *this;
  }
  template <typename... TArgs>
  self_type &operator>>(std::set<TArgs...> &data) {
    size_t num;
    *this >> num;
    data.clear();
    for (size_t i = 0; i < num; i++) {
      typename std::set<TArgs...>::value_type v;
      *this >> v;
      data.insert(std::move(v));
    }
    return *this;
  }
  template <typename... TArgs>
  self_type &operator>>(std::map<TArgs...> &data) {
    size_t num;
    *this >> num;
    for (size_t i = 0; i < num; i++) {
      typename std::map<TArgs...>::key_type k;
      typename std::map<TArgs...>::mapped_type v;
      *this >> k >> v;
      data.insert_or_assign(std::move(k), std::move(v));
    }
    return *this;
  }
  template <typename T>
  self_type &operator>>(std::optional<T> &data) {
    return *this>>data.value();
  }
};

template <typename T>
T load(const std::filesystem::path& path)
{
  auto ifs = std::ifstream{path};
  T data;
  loader(std::istream_iterator<size_t>{ifs})>>data;
  return data;
}

// class Workflow
// {
//   template <typename T>
//   static std::vector<std::optional<T>> load_data(const std::filesystem::path &path, const std::source_location location = std::source_location::current())  
//   {
//     rich::console::rule(location.function_name());
//     std::vector<std::optional<T>> result;
//     std::optional<T> data;
//     auto ifs = std::ifstream{path};
//     auto ifs_iter = std::istream_iterator<size_t>{ifs};
//     for(auto i: std::views::iota((size_t)0,*ifs_iter)|rich::views::track())
//     {
//       data = load<T>(ifs_iter);
//       // co_return data;
//       result.push_back(std::move(data));
//     }
//     return result;
//   }
// };
