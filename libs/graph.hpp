#include <algorithm>
#include <concepts>
#include <functional>
#include <iterator>
#include <map>
#include <memory>
#include <numeric>
#include <ranges>
#include <set>
#include <span>
#include <utility>
#include <vector>

template <typename T>
class graph {
  using vert_t = T;
  using edge_t = std::tuple<vert_t, vert_t>;
  std::map<const vert_t,std::set<const vert_t>> data;
public:
  void remove_vert(const vert_t& v){
    auto node = data.extract(v);
    for (auto &&u: node.mapped())
      remove_edge({u,v});
    data.erase(v);
  }
  void remove_edge(const edge_t &e)
  {
    auto &&[u,v] = e;
    if(auto it_u = data.find(u); it_u != data.end())
      if(auto it_v = it_u->second.find(v); it_v!= it_u->second.end())
        it_u->second.erase(it_v);
  }
  void add_edge(const edge_t &e){
    auto &&[u,v] = e; 
    data[u].insert(v);
  }
  void add_vert(const vert_t &v){
    if(!data.contains(v))
      data[v] = {};
  }
  const std::set<const vert_t> neighbors_of(const vert_t &v) const{
    return data[v];
  }
};

// template <std::unsigned_integral T>
// class graph {
//   using vert_t = T;
//   using edge_t = std::pair<T, T>;
//   std::set<vert_t> m_verts;
//   std::set<edge_t> m_edges;
// public:
//   static constexpr vert_t INVALID_VERT = static_cast<vert_t>(-1);
//   size_t num_vert() { return m_verts.size(); }
//   size_t num_edge() { return m_edges.size(); }
//   void add_vert(vert_t vert) { m_verts.insert(vert); }
//   void add_edge(edge_t edge) {
//     auto [u, v] = edge;
//     m_edges.insert(edge);
//     add_vert(u);
//     add_vert(v);
//   }
//   graph& operator|=(const graph &other)
//   {
//     std::ranges::copy(other.m_verts,std::inserter(m_verts,m_verts.end()));
//     std::ranges::copy(other.m_edges,std::inserter(m_edges,m_edges.end()));
//     return *this;
//   }
//   std::vector<std::vector<vert_t>> to_adjacent_array() const {
//     vert_t num = std::ranges::max(m_verts)+1;
//     auto result = std::vector<std::vector<vert_t>>(num);
//     for(auto [u,v]: m_edges)
//       result[u].push_back(v);
//     return result;
//   }

//   std::vector<size_t> core_value() const {
//     auto adj = to_adjacent_array();
//     auto cur_deg = std::vector<size_t>(adj.size());
//     for (vert_t v =0; v<adj.size(); v++)
//       cur_deg[v] = adj[v].size();
//     size_t max_deg = std::ranges::max(cur_deg);
//     auto bin = std::vector<size_t>(max_deg+1);
//     auto pos = std::vector<size_t>(adj.size());
//     auto vert = std::vector<size_t>(adj.size());
//     auto bucket = std::vector<std::vector<vert_t>>(max_deg+1);
//     for (vert_t v=0;v<adj.size();v++)
//       bucket[cur_deg[v]].push_back(v);
//     std::ranges::copy(bucket|std::views::join,vert.begin());
//     for (size_t i = 0; i<max_deg+1;i++)
//       bin[i]=bucket[i].size();
//     std::partial_sum(bin.begin(),bin.end(),bin.begin());
//     for (vert_t v=0;v<adj.size();v++)
//       pos[vert[v]]=v;
//     for (size_t i = 0;i<max_deg; i++)
//       bin[max_deg-i] = bin[max_deg-i-1];
//     bin[0]=0;
//     auto result = std::vector<size_t>(adj.size());
//     for (size_t i = 0; i < adj.size(); i++) {
//       vert_t v = vert[i];
//       result[v] = cur_deg[v];
//       for (auto u : adj[v]) {
//         if (cur_deg[u] != cur_deg[v]) {
//           vert_t w = vert[bin[cur_deg[u]]];
//           std::swap(vert[pos[u]], vert[pos[w]]);
//           std::swap(pos[u], pos[w]);
//           cur_deg[u]--;
//         }
//       }
//     }
//     return result;
//   }
// };
