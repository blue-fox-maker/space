#pragma once
#include <execution>
#include <functional>
#include <limits>
#include <source_location>
#include <optional>
#include <ranges>
#include <string>
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

namespace rich::style{
const std::string set_bold        ="\033[1m";
const std::string set_dim         ="\033[2m";
const std::string set_italic      ="\033[3m";
const std::string set_underline   ="\033[4m";
const std::string set_blink       ="\033[5m";
const std::string set_reverse     ="\033[7m";
const std::string set_conceal     ="\033[8m";
const std::string set_strike      ="\033[9m";
const std::string set_black       ="\033[30m";
const std::string set_red         ="\033[31m";
const std::string set_green       ="\033[32m";
const std::string set_yellow      ="\033[33m";
const std::string set_blue        ="\033[34m";
const std::string set_magenta     ="\033[35m";
const std::string set_cyan        ="\033[36m";
const std::string set_white       ="\033[37m";
const std::string reset           ="\033[0m";
const std::string reset_bold      ="\033[22m";
const std::string reset_dim       ="\033[22m";
const std::string reset_italic    ="\033[23m";
const std::string reset_underline ="\033[24m";
const std::string reset_blink     ="\033[25m";
const std::string reset_reverse   ="\033[27m";
const std::string reset_conceal   ="\033[28m";
const std::string reset_strike    ="\033[29m";
const std::string reset_fg        ="\033[39m";
}

namespace rich{
template <typename T>
struct renderable{
  const T& data;  
};
template<class U>
renderable(U) -> renderable<U>;
}

template <typename T> requires std::is_arithmetic_v<T> 
struct std::formatter<rich::renderable<T>>:public std::formatter<T>{
  template <typename FmtContext>
  auto format(const rich::renderable<T> val, FmtContext &ctx) const {
    *ctx.out()++ = rich::style::set_magenta;
    std::formatter<T>::format(val.data,ctx);
    *ctx.out()++ = rich::style::reset_fg;
    return ctx.out();
  }
};
template <>
struct std::formatter<rich::renderable<std::string>>:public std::formatter<std::string>{
  template <typename FmtContext>
  auto format(const rich::renderable<std::string>& val, FmtContext &ctx) const {
    *ctx.out()++ = rich::style::set_green + "\"";
    std::formatter<std::string>::format(val.data,ctx);
    *ctx.out()++ = "\"" + rich::style::reset_fg;
    return ctx.out();
  }
};
template <>
struct std::formatter<rich::renderable<std::string_view>>:public std::formatter<std::string_view>{
  template <typename FmtContext>
  auto format(const rich::renderable<std::string_view>& val, FmtContext &ctx) const {
    *ctx.out()++ = rich::style::set_green;
    std::formatter<std::string_view>::format(val.data,ctx);
    *ctx.out()++ = rich::style::reset_fg;
    return ctx.out();
  }
};
template <>
struct std::formatter<rich::renderable<char>>:public std::formatter<char>{
  template <typename FmtContext>
  auto format(const rich::renderable<char> val, FmtContext &ctx) const {
    *ctx.out()++ = rich::style::set_yellow + "\'";
    std::formatter<char>::format(val.data,ctx);
    *ctx.out()++ = "\'" + rich::style::reset_fg;
    return ctx.out();
  }
};
template <>
struct std::formatter<rich::renderable<std::filesystem::path>>:public std::formatter<std::string>{
  template <typename FmtContext>
  auto format(const rich::renderable<std::filesystem::path>& val, FmtContext &ctx) const {
    *ctx.out()++ = rich::style::set_cyan;
    *ctx.out()++ = val.data.string();
    *ctx.out()++ = rich::style::reset_fg;
    return ctx.out();
  }
};
template <>
struct std::formatter<rich::renderable<std::source_location>>:public std::formatter<std::string>{
  template <typename FmtContext>
  auto format(const rich::renderable<std::source_location>& val, FmtContext &ctx) const {
    *ctx.out()++ = rich::style::set_red;   
    std::format_to(ctx.out(),"{}({}:{}) {}",val.data.file_name(), val.data.line(), val.data.column(),val.data.function_name());
    *ctx.out()++ = rich::style::reset_fg;
    return ctx.out();
  }
};
template <typename ...Args>
struct std::formatter<rich::renderable<std::chrono::duration<Args...>>>:public std::formatter<std::chrono::duration<Args...>>{
  template <typename FmtContext>
  auto format(const rich::renderable<std::chrono::duration<Args...>>& val, FmtContext &ctx) const {
    *ctx.out()++ = rich::style::set_blue;
    std::formatter<std::chrono::duration<Args...>>::format(val.data,ctx);
    *ctx.out()++ = rich::style::reset_fg;
    return ctx.out();
  }
};
template <typename T>
struct std::formatter<rich::renderable<std::chrono::hh_mm_ss<T>>>:public std::formatter<std::chrono::hh_mm_ss<T>>{
  template <typename FmtContext>
  auto format(const rich::renderable<std::chrono::hh_mm_ss<T>>& val, FmtContext &ctx) const {
    *ctx.out()++ = rich::style::set_blue;
    std::formatter<std::chrono::hh_mm_ss<T>>::format(val.data,ctx);
    *ctx.out()++ = rich::style::reset_fg;
    return ctx.out();
  }
};
template <std::ranges::random_access_range T> 
struct std::formatter<rich::renderable<T>>: public std::formatter<std::string>{
  template <typename FmtContext>
  auto format(const rich::renderable<T>& val, FmtContext &ctx) const {
    *ctx.out()++ = "[";
    for (std::string delim = ""; const auto &v : val.data)
      std::format_to(ctx.out(),"{}{}",delim,rich::renderable{v}), delim = ", ";
    *ctx.out()++ = "]";
    return ctx.out();
  }
};
template <std::ranges::range T> 
struct std::formatter<rich::renderable<T>>: public std::formatter<std::string>{
  template <typename FmtContext>
  auto format(const rich::renderable<T>& val, FmtContext &ctx) const {
    *ctx.out()++ = "{";
    for (std::string delim = ""; const auto &v : val.data)
      std::format_to(ctx.out(),"{}{}",delim,rich::renderable{v}), delim = ", ";
    *ctx.out()++ = "}";
    return ctx.out();
  }
};
template <typename ...Args>
struct std::formatter<rich::renderable<std::tuple<Args...>>>: public std::formatter<std::string>{
  template <typename FmtContext>
  auto format(const rich::renderable<std::tuple<Args...>>& val, FmtContext &ctx) const {
    *ctx.out()++ = "(";
    std::apply([&](auto&&...args){
      std::string delim = "";
      ((std::format_to(ctx.out(),"{}{}",delim,rich::renderable(args)),delim=", "),...);
    },val.data);
    *ctx.out()++ = ")";
    return ctx.out();
  }
};
template <typename ...Args>
struct std::formatter<rich::renderable<std::pair<Args...>>>: public std::formatter<std::string>{
  template <typename FmtContext>
  auto format(const rich::renderable<std::pair<Args...>>& val, FmtContext &ctx) const {
    return std::format_to(ctx.out(),"<{}:{}>",rich::renderable{val.data.first},rich::renderable{val.data.second});
  }
};

namespace rich::io {
template <typename T>
bool _load(std::istream&,T&);
template <typename T>
bool _load(std::istream&,std::optional<T>&);
template <typename T1, typename T2>
bool _load(std::istream&, std::pair<T1,T2>&);
template <typename ...Args>
bool _load(std::istream&, std::tuple<Args...>&);
template <typename ...Args>
bool _load(std::istream&, std::vector<Args...>&);
template <typename T, typename ...Args>
bool _load(std::istream&, std::set<T,Args...>&);

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
template <typename T>
bool _load(std::istream& is, std::optional<T>&val){
  if(T temp;_load(is,temp))
  {
    val = std::move(temp); 
    return true;
  }
  return false;
}
template <typename T>
std::optional<T> load(std::istream& is = std::cin){
  std::optional<T> result;
  return _load(is,result)? result: std::nullopt; 
}
}


namespace rich {

class basic_console {
public:
  std::istream &is = std::cin;
  std::ostream &os = std::cout;
  const std::string newline = "  \n";

  void println(std::string_view context = "", auto &&...args){
    os << std::vformat(std::move(context),std::make_format_args(renderable{std::forward<decltype(args)>(args)}...)) << newline <<std::flush;      
  }
  void print(std::string_view context = "", auto &&...args){
    os << std::vformat(std::move(context),std::make_format_args(renderable{std::forward<decltype(args)>(args)}...)) <<std::flush;      
  }
  void render(std::string_view context = "", auto &&...args){
    println(context,std::forward<decltype(args)>(args)...);
  }
  template <typename T>
  T read(const std::function<bool(const T&)>& verify,std::string&& prompt, auto &&...args){
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
  // template <typename Func, std::ranges::range R>
  // void bench(const Func& func, R&& args_range, std::source_location location = std::source_location::current()){
  //   println("benchmark at {}",location);
  //   auto time_start = std::chrono::system_clock::now();
  //   for (auto &&args_tuple: track(std::forward<R>(args_range))){
  //     std::apply([&](auto&&...args){func(std::forward<decltype(args)>(args)...);}, std::forward<decltype(args_tuple)>(args_tuple));
  //   }
  //   auto time_cost = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - time_start); 
  // }
  template <std::invocable F>
  std::invoke_result_t<F> bench(F&& func,std::string_view description = "function call", std::source_location location = std::source_location::current()) noexcept(std::is_nothrow_invocable_v<F>){
    auto time_lower = std::chrono::steady_clock::now();
    std::invoke(std::forward<F>(func));
    println("{} at {} takes {}",description,location,std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()-time_lower));
  }
private:
  void progress(size_t num_step, size_t cur_step, std::chrono::hh_mm_ss<std::chrono::seconds> duration, std::string_view description) {
    size_t percentage = cur_step * 100 / num_step;
    const std::array<std::string_view, 6> spinner = {"⠋", "⠙", "⠸", "⠴", "⠦", "⠇"};
    std::string bar;
    constexpr size_t WIDTH = 30; 
    for (size_t i = 0; i < WIDTH; i++)
      bar.append(i<WIDTH*percentage/100? "━": " ");
    const bool finished = percentage == 100;
    os <<std::format("\r{} {} {} {:3}% {}{}", finished? std::string_view("✓") : spinner[cur_step % spinner.size()], renderable{duration}, bar, renderable{percentage}, description, finished?"\n":"") << std::flush;
  }
};

static basic_console console; 
}; // namespace rich
