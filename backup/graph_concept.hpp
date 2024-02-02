#include "dbg.h"
#include <algorithm>
#include <array>
#include <bitset>
#include <compare>
#include <concepts>
#include <initializer_list>
#include <iterator>
#include <map>
#include <memory>
#include <ranges>
#include <set>
#include <span>
#include <vector>

class index_bilinked_list;
class adjacent_matrix;
class adjacent_array;
class adjacent_list;
template <typename T,size_t N>
class level_list;

using count_t = size_t;

template <typename T>
concept Graph = requires(T g) {
  typename T::vert_t;
  typename T::edge_t;
  { T::vert_t } -> std::swappable_with<size_t>;
};


template <typename T>
concept VertGraph = Graph<T> && requires(T g) {
  { g.neighbors(T::vert_t) } -> std::ranges::sized_range;
  { g.verts() } -> std::ranges::sized_range;
};
template <typename T>
concept VertGraphView = Graph<T> && requires(T g) {
  { g.neighbors(T::vert_t) } -> std::ranges::range;
  { g.verts() } -> std::ranges::sized_range;
};

class index_bilinked_list {
  using count_t = size_t;

  class const_interator {
    count_t m_index;
    const index_bilinked_list *m_ptr;

  public:
    const_interator(const count_t index, const index_bilinked_list *ptr) : m_ptr(ptr), m_index(index) {}
    const_interator() = default;
    const_interator &operator++() {
      m_index = m_ptr->m_next_ptr[m_index];
      return *this;
    }
    const_interator operator++(int) {
      const_interator temp = *this;
      ++(*this);
      return temp;
    }
    const_interator &operator--() {
      m_index = m_ptr->m_prev_ptr[m_index];
      return *this;
    }
    const_interator operator--(int) {
      const_interator temp = *this;
      --(*this);
      return temp;
    }
    const count_t operator*() const {
      return m_index;
    }
    friend auto operator<=>(const const_interator &a, const const_interator &b) = default;
  };

  std::unique_ptr<count_t[]> m_prev_ptr = nullptr;
  std::unique_ptr<count_t[]> m_next_ptr = nullptr;
  count_t m_index_l = 0;
  count_t m_index_r = 0;
  count_t m_offset = 0;
  count_t m_capacity = 0;
  count_t m_size = 0;
  constexpr count_t index(const count_t idx) const noexcept { return idx - m_offset; }
  constexpr count_t excessive_index_l() const noexcept { return static_cast<count_t>(-1); }
  constexpr count_t excessive_index_r() const noexcept { return m_capacity; }

public:
  index_bilinked_list() = delete;
  index_bilinked_list(index_bilinked_list &) = delete;
  index_bilinked_list(index_bilinked_list &&) = default;

  index_bilinked_list(std::span<count_t> index_list) {
    if (index_list.empty())
      return;
    m_size = index_list.size();
    m_offset = index_list.front();
    m_capacity = index_list.back() - m_offset + 1;
    m_index_l = 0;
    m_index_r = m_capacity - 1;
    m_prev_ptr = std::make_unique<count_t[]>(m_capacity);
    m_next_ptr = std::make_unique<count_t[]>(m_capacity);
    for (count_t i = 0, j = 1; j < index_list.size(); i++, j++) {
      count_t prev_index = index_list[i] - m_offset;
      count_t next_index = index_list[j] - m_offset;
      m_prev_ptr[next_index] = prev_index;
      m_next_ptr[prev_index] = next_index;
    }
    m_prev_ptr[m_index_l] = excessive_index_l();
    m_next_ptr[m_index_r] = excessive_index_r();
  }
  void undo_deactivate(const count_t idx) {
    count_t pos = index(idx);
    if (m_prev_ptr[pos] != excessive_index_l())
      m_next_ptr[m_prev_ptr[pos]] = pos;
    else
      m_index_l = pos;
    if (m_next_ptr[pos] != excessive_index_r())
      m_prev_ptr[m_next_ptr[pos]] = pos;
    else
      m_index_r = pos;
    m_size++;
  }
  void deactivate(const count_t idx) {
    count_t pos = index(idx);
    if (m_prev_ptr[pos] != excessive_index_l())
      m_next_ptr[m_prev_ptr[pos]] = m_next_ptr[pos];
    else
      m_index_l = m_next_ptr[pos];
    if (m_next_ptr[pos] != excessive_index_r())
      m_prev_ptr[m_next_ptr[pos]] = m_prev_ptr[pos];
    else
      m_index_r = m_prev_ptr[pos];
    m_size--;
  }
  [[nodiscard]] constexpr bool empty() const noexcept { return m_index_l == excessive_index_r(); }
  [[nodiscard]] const_interator begin() const noexcept { return {m_index_l, this}; }
  [[nodiscard]] const_interator end() const noexcept { return {excessive_index_r(), this}; }
  [[nodiscard]] const_interator rbegin() const noexcept { return {m_index_r, this}; }
  [[nodiscard]] const_interator rend() const noexcept { return {excessive_index_l(), this}; }
  [[nodiscard]] count_t size() const noexcept { return m_size; }
};

class adjacent_matrix {
  using vert_t = size_t;
  using edge_t = std::pair<vert_t, vert_t>;
  std::vector<index_bilinked_list> m_data;
  template <VertGraphView G>
  adjacent_matrix(G &&graph) : m_data(graph.verts().size()) {
    for (auto &&v : graph.verts())
      graph[v].emplace(std::forward<G>(graph).neighbors(v));
  }
  [[nodiscard]] constexpr const auto &verts() const noexcept { return std::views::iota(1, m_data.size()); }
  [[nodiscard]] constexpr auto &neighbors(const vert_t v) const &noexcept { return m_data[v]; }
  [[nodiscard]] auto &&neighbors(const vert_t v) &&noexcept { return m_data[v]; }
};

class adjacent_list {
  using vert_t = size_t;
  using edge_t = std::pair<vert_t, vert_t>;
  std::vector<std::set<vert_t>> m_data;
  adjacent_list(count_t num_vert) : m_data(num_vert) {}
  template <VertGraphView G>
  adjacent_list(G &&graph) : m_data(graph.verts().size()) {
    for (auto &&v : graph.verts())
      std::ranges::copy(std::forward<G>(graph).neighbors(v), std::inserter(m_data[v]));
  }
  [[nodiscard]] constexpr const auto &verts() const noexcept { return std::views::iota(1, m_data.size()); }
  [[nodiscard]] const auto &neighbors(const vert_t v) const &noexcept { return m_data[v]; }
  [[nodiscard]] auto neighbors(const vert_t v) &&noexcept { return m_data[v]; }
  void add_edge(const vert_t &u, const vert_t &v) {
    m_data[u].insert(v);
  }
};

template <VertGraph G>
class induced_view {
  using vert_t = typename G::vert_t;
  using edge_t = typename G::edge_t;
  using size_type = size_t;
  const G &m_graph;
  std::set<vert_t> m_data;
  induced_view(const G &graph, std::set<vert_t> &&data) : m_graph(graph), m_data(std::move(data)) {}
  [[nodiscard]] const auto &verts() const noexcept { return m_data; }
  [[nodiscard]] const auto &neighbors(const vert_t &v) const noexcept {
    return m_graph.neighbors(v) | std::views::filter([&](const vert_t &u) { return m_data.contains(u); });
  }
};

template <VertGraph G>
class remapped_view {
  using vert_t = typename G::vert_t;
  using edge_t = typename G::edge_t;
  using size_type = size_t;
  const G &m_graph;
  std::map<vert_t, vert_t> m_data;
  remapped_view(const G &graph, std::map<vert_t, vert_t> &&data) : m_graph(graph), m_data(data) {}
  [[nodiscard]] const auto &verts() const noexcept {
    return m_graph.verts() | std::views::transform([&](const vert_t &u) { return m_data[u]; });
  }
  [[nodiscard]] const auto &neighbors(const vert_t &v) const noexcept {
    return m_graph.neighbors(m_data[v]) | std::views::transform([&](const vert_t &u) { return m_data[u]; });
  }
};

template <VertGraph G>
bool is_plex(const G &graph, size_t k)
{
  return std::ranges::all_of(graph.verts(),[&](const typename G::vert_t &v){return graph.neighbors(v).size()+k>=graph.verts().size();});
}

template <VertGraph G>
bool is_clique(const G &graph)
{
  return is_plex(graph,1);
}

// template <typename T, size_t N>
// class level_list
// {
//   using value_type = T;
//   std::array<count_t, N> m_index;
//   std::vector<value_type> m_data;
// };

template <VertGraph G>
void BK(const G&graph, const size_t k, std::span<typename G::vert_t> P, std::span<std::pair<typename G::vert_t, count_t>> C, std::span<std::pair<typename G::vert_t, count_t>> X)
{
  // P = span {0, index_P};
  // C = span {index_P, index_C};
  // X = span {index_C, index_X};
  // degree_list;
  if(X.empty() and C.empty())
    dbg(P);
  for (auto && v: C)
  {
    // C to P
    // change degree_list

    count_t num_removed_C = 0;
    for (count_t i = 0; i < C.size(); i++)
    {
      auto [v, degree] = C[i];
      if (degree+k < P.size())
      {
        std::swap(C[i],C.back());
        num_removed_C++;
      }
    }
    // so does X
    BK(graph,k,degree_list, P,C,X);
    C.undo();
    // so dose X
  }
}
