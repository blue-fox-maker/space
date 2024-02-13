#include <algorithm>
#include <concepts>
#include <map>
#include <ranges>
#include <set>
#include <tuple>
#include <vector>

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
};

template <typename TIter>
class dumper {
  TIter iter;
  using self_type = dumper<TIter>;

public:
  dumper(const TIter &iter) : iter(iter){};
  template <typename T>
  self_type &operator<<(T &&data) {
    *iter++ = std::forward(data);
    return *this;
  }
  template <typename T1, typename T2>
  self_type &operator<<(std::pair<T1, T2> &&data) {
    return *this << std::forward(data.first) << std::forward(data.second);
  }
  template <typename... TArgs>
  self_type &operator<<(std::tuple<TArgs...> &&data) {
    std::apply([this](auto &&...args) { ((*this << args), ...); });
    return *this;
  }
  template <std::ranges::sized_range R>
  self_type &operator<<(R &&data) {
    *this << data.size();
    for (auto &&v : data)
      *this << std::forward(v);
  }
};

template <typename TIter>
auto load(const TIter &iter) {
  return loader<TIter>(iter);
}

template <typename TIter>
auto dump(const TIter &iter) {
  return dumper<TIter>(iter);
}
