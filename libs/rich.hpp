#pragma once
#include <functional>
#include <limits>
#include <source_location>
#include <optional>
#include <ranges>
#include <stacktrace>
#include <type_traits>
#include <utility>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <iterator>
#include <vector>
#include <map>
#include <set>
#include <exception>
#include <filesystem>
#include <format>
#include <iostream>

namespace rich::io {

template <typename T>
std::optional<T> load(std::istream& is = std::cin){
  std::optional<T> result;
  return _load(is,result.value())? result: std::nullopt; 
}
template <typename T>
bool _load(std::istream& is,T& val){
  is >> val;
  if(!is.good()){
    is.clear();      
    is.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
    return false;
  }
  return true;
}
template <typename T1, typename T2>
bool _load(std::istream& is, std::pair<T1,T2>& val){
  return _load(is,val.first)&&_load(is,val.second);
}
template <typename ...Args>
bool _load(std::istream& is, std::tuple<Args...>& val){
  return std::apply([&is](auto&&...args){
    return (_load(is,args)&&...);        
  },val);  
}
template <typename ...Args>
bool _load(std::istream& is, std::vector<Args...>& val){
  size_t count;   
  if(!_load(is, count)) return false;
  val.resize(count);
  for (auto& x: val) if(!_load(is,x)) return false;
  return true;
}
template <typename T, typename ...Args>
bool _load(std::istream& is, std::set<T,Args...>& val){
  size_t count;   
  if(!_load(is,count)) return false;
  for (auto i: std::views::iota(0U,count))
  {
    T value;
    if(!_load(is,count)) return false;
    val.insert(std::move(value));
  }
  return true;
}
}

// namespace rich::io::dep {
// template <typename TIter>
// class loader {
//   TIter iter;
//   using self_type = loader<TIter>;

// public:
//   loader(const TIter &iter) : iter(iter){};
//   template <typename T>
//   self_type &operator>>(T &data) {
//     data = *iter++;
//     return *this;
//   }
//   template <typename T1, typename T2>
//   self_type &operator>>(std::pair<T1, T2> &data) {
//     return *this >> data.first >> data.second; 
//   }
//   template <typename... TArgs>
//   self_type &operator>>(std::tuple<TArgs...> &data) {
//     std::apply([&](auto &...args) { (*this >> ... >> args); }, data);
//     return *this;
//   }
//   template <typename... TArgs>
//   self_type &operator>>(std::vector<TArgs...> &data) {
//     size_t num;
//     *this >> num;
//     data.resize(num);
//     for (size_t i = 0; i < num; i++)
//       *this >> data[i];
//     return *this;
//   }
//   template <typename... TArgs>
//   self_type &operator>>(std::set<TArgs...> &data) {
//     size_t num;
//     *this >> num;
//     data.clear();
//     for (size_t i = 0; i < num; i++) {
//       typename std::set<TArgs...>::value_type v;
//       *this >> v;
//       data.insert(std::move(v));
//     }
//     return *this;
//   }
//   template <typename... TArgs>
//   self_type &operator>>(std::map<TArgs...> &data) {
//     size_t num;
//     *this >> num;
//     for (size_t i = 0; i < num; i++) {
//       typename std::map<TArgs...>::key_type k;
//       typename std::map<TArgs...>::mapped_type v;
//       *this >> k >> v;
//       data.insert_or_assign(std::move(k), std::move(v));
//     }
//     return *this;
//   }
//   template <typename T>
//   self_type &operator>>(std::optional<T> &data) {
//     return *this >> data.value();
//   }
// };
// }

namespace rich::style{
constexpr std::string bold      (std::string &&context){  return "\033[1m"+context+"\033[22m"; }
constexpr std::string dim       (std::string &&context){  return "\033[2m"+context+"\033[22m"; }
constexpr std::string italic    (std::string &&context){  return "\033[3m"+context+"\033[23m"; }
constexpr std::string underline (std::string &&context){  return "\033[4m"+context+"\033[24m"; }
constexpr std::string blink     (std::string &&context){  return "\033[5m"+context+"\033[25m"; }
constexpr std::string reverse   (std::string &&context){  return "\033[7m"+context+"\033[27m"; }
constexpr std::string conceal   (std::string &&context){  return "\033[8m"+context+"\033[28m"; }
constexpr std::string strike    (std::string &&context){  return "\033[9m"+context+"\033[29m"; }
constexpr std::string black     (std::string &&context){  return "\033[30m"+context+"\033[39m"; }
constexpr std::string red       (std::string &&context){  return "\033[31m"+context+"\033[39m"; }
constexpr std::string green     (std::string &&context){  return "\033[32m"+context+"\033[39m"; }
constexpr std::string yellow    (std::string &&context){  return "\033[33m"+context+"\033[39m"; }
constexpr std::string blue      (std::string &&context){  return "\033[34m"+context+"\033[39m"; }
constexpr std::string magenta   (std::string &&context){  return "\033[35m"+context+"\033[39m"; }
constexpr std::string cyan      (std::string &&context){  return "\033[36m"+context+"\033[39m"; }
constexpr std::string white     (std::string &&context){  return "\033[37m"+context+"\033[39m"; }
}

namespace rich {

class basic_console {
public:
  std::ostream &os = std::cout;
  std::istream &is = std::cin;

  // void render() { os << "\n"; }
  void render(std::string&& context = "" , auto &&...args) {
    auto result = std::move(context);
    if(result.starts_with("# ")) result = "# " + style::bold(style::underline(std::move(result).substr(2)));   
    if(result.starts_with("- ")) result = "  • " + result.substr(2);
    if(result.starts_with("> ")) result = "  ┃ " + style::italic(std::move(result).substr(2));
    println(std::move(result),std::forward<decltype(args)>(args)...);
  }
  template <typename T>
  T read(std::function<bool(const T&)> verify,std::string&& prompt, auto &&...args){
    print(std::move(prompt)+": ",std::forward<decltype(args)>(args)...);
    T res;
    while(!io::_load(is,res)||!verify(res)){
      print("invalid input, please try again: ");
    }
    return res;
  }
  template <typename T>
  T read(std::string&& prompt, auto &&...args){
    return read<T>([](const auto&)noexcept{return true;}, std::move(prompt) ,std::forward<decltype(args)>(args)...);
  }
    
  template <std::ranges::sized_range R>
  auto track(R&& rng, std::string_view description = ""){
      auto total_step = std::ranges::size(rng);
      auto start_time = std::chrono::system_clock::now();
      return rng|std::views::enumerate| std::views::transform([=,this](auto&&x){
        auto &&[i,v] = x;
        progress(total_step,i+1,std::chrono::hh_mm_ss{std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now()-start_time)},description);
        return std::forward<decltype(v)>(v);});
  }
  template <typename Func, std::ranges::range R>
  void bench(const Func& func, R&& args_range, std::source_location location = std::source_location::current()){
    render("benchmark at {}",location);
    auto time_start = std::chrono::system_clock::now();
    for (auto &&args_tuple: track(std::forward<R>(args_range))){
      std::apply([&](auto&&...args){func(std::forward<decltype(args)>(args)...);}, std::forward<decltype(args_tuple)>(args_tuple));
    }
    auto time_cost = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - time_start); 
  }

private:
  void println(std::string_view context, auto &&...args) {
    os << std::vformat(context, std::make_format_args(pretty_print(std::forward<decltype(args)>(args))...)) << std::endl;
  } 
  void print(std::string_view context, auto &&...args) {
    os << std::vformat(context, std::make_format_args(pretty_print(std::forward<decltype(args)>(args))...)) << std::flush;
  } 
  void progress(size_t num_step, size_t cur_step, std::chrono::hh_mm_ss<std::chrono::seconds> duration, const std::string_view description) {
    size_t percentage = cur_step * 100 / num_step;
    constexpr size_t width = 30;
    const std::array<std::string, 6> spinner = {"⠋", "⠙", "⠸", "⠴", "⠦", "⠇"};
    std::string bar;
    for (size_t i = 0; i < width; i++)
      bar.append( i<percentage * width/100 ? "━" : " ");
    os << std::format("\r{} {} {} {:3}% {}{}",
                        percentage == 100 ? "✓" : spinner[cur_step % spinner.size()],
                        pretty_print(duration),
                        bar,
                        percentage,
                        pretty_print(description),
                        percentage == 100 ? "\n" : ""
                     )<<std::flush;
  }
  template <typename ...Args>
  constexpr std::string pretty_print(const std::chrono::duration<Args...> &val) const {
    return style::blue(std::format("{}",val));   
  }
  template <typename ...Args>
  constexpr std::string pretty_print(const std::chrono::hh_mm_ss<Args...> &val) const {
    return style::blue(std::format("{}", val));   
  }
  constexpr std::string pretty_print(const std::filesystem::path &val) const {
    return style::green(style::underline(val.string()));  
  }
  constexpr std::string pretty_print(const std::source_location &val) const {
    return style::green(std::format("{}({}:{}) {}", val.file_name(), val.line(), val.column(), val.function_name()));   
  }
  template <typename T> requires std::is_arithmetic_v<T>
  constexpr std::string pretty_print(const T val) const{
    return style::magenta(std::to_string(val)); 
  }
  constexpr std::string pretty_print(const std::string_view val) const{
    return style::cyan(std::string(val));   
  }
  constexpr std::string pretty_print(const std::string &val) const{
    return style::cyan("\"" +val+ "\"");   
  }
  constexpr std::string pretty_print(const char val) const{
    return style::yellow({'\'' , val , '\''});   
  }
  template <std::ranges::random_access_range R>
  constexpr std::string pretty_print(const R& val) const {
    std::string result;
    result += "[";
    for (std::string delim = ""; const auto &v : val)
      result += delim + pretty_print(v) ,delim = ", ";
    result += "]";
    return result;
  }
  template <std::ranges::range R>
  constexpr std::string pretty_print(const R &val) const {
    std::string result;
    result += "{";
    for (std::string delim = ""; const auto &v : val)
      result += delim + pretty_print(v), delim = ", ";
    result += "}";
    return result;
  }
  template <typename... TArgs>
  constexpr std::string pretty_print(const std::tuple<TArgs...> &val) const {
    return std::apply([&](const TArgs &...v) {
      std::string result;
      result += "(";
      std::string delim = "";
      ((result += delim + pretty_print(v), delim = ", "), ...);
      result += ")";
      return result;
    },val);
  }
  template <typename T, typename U>
  constexpr std::string pretty_print(const std::pair<T, U> &val) const {
    return "<" + pretty_print(val.first) + ": " + pretty_print(val.second) + ">";
  }


};

static basic_console console; 
}; // namespace rich

