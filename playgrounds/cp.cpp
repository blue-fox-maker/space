#include <cassert>
#include <algorithm>
#include <iterator>
#include <ranges>
#include <span>
#include <vector>

int main()
{
  auto vec = std::vector{1,2,3,4,5,6};
  auto temp = decltype(vec){};
  auto sp = std::span{vec};
  std::ranges::copy(sp,std::back_inserter(temp));
  assert(!sp.empty());
  assert(!temp.empty());
}
