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
