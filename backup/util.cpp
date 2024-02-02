#pragma once
#include <concepts>
#include <iterator>
#include <vector>

// leetcode 405094 combination
std::vector<std::vector<size_t>> combination(size_t n, size_t k)
{
  std::vector<size_t> temp;
  std::vector<std::vector<size_t>> result;
  for (size_t i = 1; i<= k; i++)
    temp.push_back(i);
  temp.push_back(n+1);
  int j = 0;
  while(j<k){
    result.emplace_back(temp.begin(),temp.begin()+k);
    j = 0;
    while(j<k && temp[j] +1 == temp [j+1]){
      temp[j] = j+1;
      ++j;
    }
    temp[j]++;
  }
  return result;
}
