#include <algorithm>
#include <ranges>
#include <memory>
#include <numeric>

template <std::unsigned_integral T>
class disjoint_set{
public:
  using index_type = T;
  disjoint_set(index_type size) noexcept :_parents(std::make_unique<index_type[]>(size)), _ranks(std::make_unique<index_type[]>(size)), _size(size){
    std::iota(_parents.get(),_parents.get()+size,0);
    std::iota(_ranks.get(),_ranks.get()+size,0);
  }
  [[nodiscard]] constexpr index_type find(index_type x) noexcept { return x == _parents[x]? x:(_parents[x]=find(_parents[x]));}
  [[nodiscard]] constexpr index_type find(index_type x) const noexcept { return x == _parents[x]? x: find(_parents[x]);}
  [[nodiscard]] constexpr index_type size() const noexcept{return _size;}
  [[nodiscard]] constexpr bool is_eq(index_type x, index_type y) const noexcept { return find(x)==find(y);}
  // [[nodiscard]] constexpr bool is_eq(index_type x, index_type y) noexcept { return find(x)==find(y);}
  constexpr bool merge(index_type x, index_type y) noexcept {
    x = find(x);
    y = find(y);
    if(_ranks[x]<=_ranks[y]) _parents[x]=y;
    else _parents[y]=x;
    if(x==y) return false;
    else if (_ranks[x]==_ranks[y]) _ranks[y]++;
    return true;
  }
private:
  std::unique_ptr<index_type[]> _parents; 
  std::unique_ptr<index_type[]> _ranks;
  index_type _size;
};

template <std::unsigned_integral T>
class eular_tour_forest{
public:
  using index_type = T;
  eular_tour_forest(index_type size) noexcept:_size(size), _next(std::make_unique<index_type[]>(size)){
    std::iota(_next.get(),_next.get()+size,0);
  }
  [[nodiscard]] constexpr index_type size() const noexcept { return _size;}
  [[nodiscard]] constexpr index_type next(index_type x) const noexcept { return _next[x];}
  constexpr void merge(index_type x, index_type y) noexcept {
    std::swap(_next[x],_next[y]);
  }
private:
  std::unique_ptr<index_type[]> _next;
  index_type _size;
};
