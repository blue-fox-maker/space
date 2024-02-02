#include <concepts>
#include <set>
#include <numeric>
#include <span>
#include <vector>
#include <ranges>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <istream>
#include "dbg.h"

template <typename T>
concept Graph = requires(T g)
{
  typename T::vert_t;
  typename T::edge_t;
  typename T::size_type;
};

template <typename T>
concept AdjacencyGraph = Graph<T> && requires (T g)
{
  // {g.neighbors(T::vert_t)};
  {g.verts()};
};

class adjacent_array{
  public:
  using vert_t = size_t;
  using edge_t = std::pair<vert_t,vert_t>;
  using size_type = size_t;
  private:
  std::vector<std::vector<vert_t>> m_data;
  public:
  adjacent_array(size_type num_vert):m_data(num_vert){}
  [[nodiscard]] const auto &neighbors(const vert_t& v) const noexcept {return m_data[v];}
  [[nodiscard]] const auto verts() const noexcept { return std::views::iota(static_cast<size_type>(0),m_data.size());}
  void add_edge(const vert_t& u, const vert_t &v) noexcept { m_data[u].push_back(v);}
  void add_edge(const edge_t& edge) noexcept { const auto &[u,v] = edge; add_edge(u,v);}
};

template <typename T>
T load_graph(const std::filesystem::path&);

template <>
adjacent_array load_graph<adjacent_array>(const std::filesystem::path& path)
{
  std::ifstream ifs{path};
  using vert_t = adjacent_array::vert_t;
  using edge_t = adjacent_array::edge_t;
  using size_type = adjacent_array::size_type;
  size_type num_vert, num_edge;
  ifs>>num_vert>>num_edge;
  adjacent_array graph{num_vert};  
  for (size_type i = 0; i < num_edge; i++)
  {
    vert_t u,v;
    ifs>>u>>v;
    graph.add_edge(u,v);
    graph.add_edge(v,u);
  }
  return graph;
}

template <AdjacencyGraph G>
auto core_value(const G& graph) -> std::vector<typename G::vert_t> {
  using vert_t = typename G::vert_t;
  using edge_t = typename G::edge_t;
  using size_type = typename G::size_type;
  auto cur_deg = std::vector<size_t>(graph.verts().size());
  for (auto &&v:graph.verts())
    cur_deg[v] = graph.neighbors(v).size();
  size_t max_deg = std::ranges::max(cur_deg);
  auto bin = std::vector<size_t>(max_deg + 1);
  auto pos = std::vector<size_t>(graph.verts().size());
  auto vert = std::vector<size_t>(graph.verts().size());
  auto bucket = std::vector<std::vector<vert_t>>(max_deg + 1);
  for (auto &&v:graph.verts())
    bucket[cur_deg[v]].push_back(v);
  std::ranges::copy(bucket | std::views::join, vert.begin());
  for (size_type i = 0; i < max_deg + 1; i++)
    bin[i] = bucket[i].size();
  std::partial_sum(bin.begin(), bin.end(), bin.begin());
  for (auto &&v:graph.verts())
    pos[vert[v]] = v;
  for (size_type i = 0; i < max_deg; i++)
    bin[max_deg - i] = bin[max_deg - i - 1];
  bin[0] = 0;
  auto result = std::vector<size_t>(graph.verts().size());
  dbg(bin, pos, vert, cur_deg);
  for (size_t i = 0; i < graph.verts().size(); i++) {
    vert_t v = vert[i];
    result[v] = cur_deg[v];
    for (auto u : graph.neighbors(v)) {
      if (cur_deg[u] != cur_deg[v]) {
        vert_t w = vert[bin[cur_deg[u]]];
        std::swap(vert[pos[u]], vert[pos[w]]);
        std::swap(pos[u], pos[w]);
        cur_deg[u]--;
      }
    }
  }
  return result;
}

template <AdjacencyGraph G>
G induce(const G& graph, const std::set<typename G::vert_t>& verts)
{
  std::vector<typename G::size_type> vert_to_index{graph.verts().size()};
  adjacent_array result{verts.size()};
  for(auto&&[i,v]: std::views::enumerate(verts))
    vert_to_index[v] = i;
  for(auto&& v:verts)  
    for(auto &&u:graph.neighbors(v)|std::views::filter([&](const auto& x){verts.contains(x);}))
      result.add_edge(vert_to_index[v],vert_to_index[u]);
  return result;
}


size_t rec_time = 0;
template <AdjacencyGraph G>
void bron_kerbosch(
    const G &graph, adjacent_array::size_type k, const std::function<void(const std::vector<adjacent_array::vert_t> &)> &reporter, std::vector<adjacent_array::size_type> &degree_list,
    std::vector<adjacent_array::vert_t> &P,
    std::vector<adjacent_array::vert_t> &C, std::span<adjacent_array::vert_t> C_view,
    std::vector<adjacent_array::vert_t> X) {
  rec_time++; // NOTE: HERE!!!
  for (auto iter = C_view.begin(); iter < C_view.end();) {
    if (degree_list[*iter] + k < P.size() + 1) {
      std::swap(*iter, C_view.back());
      C_view = C_view.first(C_view.size() - 1);
    } else
      iter++;
  }
  for (auto iter = X.begin(); iter < X.end();) {
    if (degree_list[*iter] + k < P.size() + 1) {
      std::swap(*iter, X.back());
      X.pop_back();
    } else
      iter++;
  }
  if (C_view.empty()) {
    if (X.empty())
      reporter(P);
  } else {
    auto v = C_view.front();

    auto new_C = C_view.last(C_view.size() - 1);
    auto new_X = X;
    new_X.push_back(v);

    P.push_back(v);
    for (auto u : graph.neighbors(v))
      degree_list[u]++;
    bron_kerbosch(graph, k, reporter, degree_list, P, C, new_C, std::move(X));

    P.pop_back();
    for (auto u : graph.neighbors(v))
      degree_list[u]--;
    bron_kerbosch(graph, k, reporter, degree_list, P, C, new_C, std::move(new_X));
  }
}

template <AdjacencyGraph G>
void bron_kerbosch_pivot(
    const G &graph, adjacent_array::size_type k, const std::function<void(const std::vector<adjacent_array::vert_t> &)> &reporter, std::vector<adjacent_array::size_type> &degree_list,
    std::vector<adjacent_array::vert_t> &P,
    std::vector<adjacent_array::vert_t> &C, std::span<adjacent_array::vert_t> C_view,
    std::vector<adjacent_array::vert_t> X) {
  rec_time++; // NOTE: HERE!!!
  for (auto iter = C_view.begin(); iter < C_view.end();) {
    if (degree_list[*iter] + k < P.size() + 1) {
      std::swap(*iter, C_view.back());
      C_view = C_view.first(C_view.size() - 1);
    } else iter++;
  }
  for (auto iter = X.begin(); iter < X.end();) {
    if (degree_list[*iter] + k < P.size() + 1) {
      std::swap(*iter, X.back());
      X.pop_back();
    } else
      iter++;
  }
  if (C_view.empty()) {
    if (X.empty())
      reporter(P);
  } else while(!C_view.empty()) {
    
    auto v = C_view.front();
    C_view = C_view.last(C_view.size() - 1);

    P.push_back(v);
    for (auto u : graph.neighbors(v))
      degree_list[u]++;

    bron_kerbosch_pivot(graph, k, reporter, degree_list, P, C, C_view, X);

    P.pop_back();
    X.push_back(v);
    for (auto u : graph.neighbors(v))
      degree_list[u]--;
  }
}

template <AdjacencyGraph G>
void D2K(const G &graph, typename G::size_type k, const std::function<void(const std::vector<typename G::vert_t> &)> &reporter)
{
  using size_type = typename G::size_type;
  using vert_t = typename G::vert_t;
  using edge_t = typename G::edge_t;
  auto core_values = core_value(graph);  
  std::vector degeneracy_ordering{graph.verts()};
  auto eta_less = [&](auto x, auto y){return degeneracy_ordering[x]<degeneracy_ordering[y];};
  std::ranges::sort(degeneracy_ordering,eta_less);
  for (auto &&v: degeneracy_ordering)
  {
    std::vector<vert_t> hop1{graph.neighbors(v)};
    hop1.push_back(v);
    std::vector<vert_t> hop2;
    for (auto &&u: hop1)
      for (auto &&w: graph.neighbors(u))
        hop2.push_back(w);
        
  }
}
