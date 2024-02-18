#pragma once
#include "disjoint_set.hpp"
#include <algorithm>
#include <chrono>
#include <format>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <ranges>

namespace rich::style {

enum class Style {
  Default,
  Black,
  Red,
  Green,
  Yellow,
  Blue,
  Magenta,
  Cyan,
  White,
  Bold,
  Dim,
  Italic,
  Underline,
  Blink,
  Reverse,
  Hide,
};

inline constexpr auto ansi(std::string_view s, std::initializer_list<Style> styles = {}) {
  auto res = std::string{s};
  for (auto style : styles) {
    res = std::format("\033[{}m{}\033[0m",
                      style == Style::Black ? 30 : style == Style::Red     ? 31
                                               : style == Style::Green     ? 32
                                               : style == Style::Yellow    ? 33
                                               : style == Style::Blue      ? 34
                                               : style == Style::Magenta   ? 35
                                               : style == Style::Cyan      ? 36
                                               : style == Style::White     ? 37
                                               : style == Style::Bold      ? 1
                                               : style == Style::Dim       ? 2
                                               : style == Style::Underline ? 4
                                               : style == Style::Blink     ? 5
                                               : style == Style::Reverse   ? 7
                                               : style == Style::Hide      ? 8
                                                                           : 39,
                      res);
  }
  return res;
}
} // namespace rich::style

namespace rich::views {

template <std::ranges::sized_range R>
class track_view : public std::ranges::view_interface<track_view<R>> {
private:
  using TIter = std::ranges::iterator_t<R>;

  struct iterator {
    using value_type = typename std::iterator_traits<TIter>::difference_type;
    using iterator_category = typename std::iterator_traits<TIter>::iterator_category;
    using reference = typename std::iterator_traits<TIter>::reference;
    using difference_type = typename std::iterator_traits<TIter>::difference_type;
    std::array<std::string, 6> spinner = {"⠋", "⠙", "⠸", "⠴", "⠦", "⠇"};
    std::string done_char = style::ansi("━", {style::Style::Red});
    std::string todo_char = style::ansi("━", {style::Style::Red, style::Style::Dim});
    std::string description = style::ansi("Working...", {style::Style::Yellow});
    std::string finished_description = style::ansi("Done      ", {style::Style::Yellow});
    std::chrono::system_clock::time_point start_time;

  private:
    TIter iter;
    difference_type length;
    difference_type num_step;
    static constexpr size_t width = 30;

  public:
    iterator(const TIter &iter, difference_type length) : iter(iter), length(length), num_step(length), start_time(std::chrono::system_clock::now()){};
    difference_type count() const { return length; }

    void update() {
      size_t percentage = (num_step - length) * 100 / num_step;
      auto duration = std::chrono::hh_mm_ss(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start_time));
      std::string bar;
      for (size_t i = 0; i < width; i++)
        bar.append(i * 100 < percentage * width ? done_char : todo_char);
      std::clog << std::format("\r\033[34m{}\033[0m \033[32m{}\033[0m {} \033[35m{:3}%\033[0m {}{}",
                               count() == 0 ? "✓" : spinner[(num_step - length) % spinner.size()],
                               duration,
                               bar,
                               percentage,
                               count() == 0 ? finished_description : description,
                               count() == 0 ? "\n" : "");
    }
    iterator &operator++() {
      ++iter;
      length--;
      update();
      return *this;
    }
    iterator &operator++(int) {
      auto temp = *this;
      ++(*this);
      return temp;
    }
    reference operator*() const {
      return *iter;
    }
    friend constexpr auto operator<=>(const iterator &a, const iterator &b) {
      return a.count() <=> b.count();
    }
    friend constexpr bool operator==(const iterator &a, std::default_sentinel_t) {
      return a.count() == 0;
    }
  };

public:
  track_view(R &&rng) : base(std::forward<R>(rng)){};
  auto begin() { return iterator{std::ranges::begin(base), std::ranges::ssize(base)}; }
  auto end() { return std::default_sentinel; }

private:
  R base;
};

struct track {
  template <std::ranges::range R>
  friend constexpr auto operator|(R &&rng, track t) {
    return track_view<R>(std::forward<R>(rng));
  }
};

} // namespace rich::views

// namespace rich::console {
//   template <typename T>
//   inline void show(const T& data){
//     std::clog<<data;
//   }
// }

namespace rich {
class console {
  std::ostream &logs;

public:
  console(std::ostream &logs = std::clog) : logs(logs) {}
  void rule(std::string context) {
    std::ranges::transform(context, context.begin(), ::toupper);
    logs << style::ansi(std::format("# [{}] ", context), {style::Style::Red}) << style::ansi(std::format("{:%T}\n", std::chrono::system_clock::now()));
  }
  void para(std::string context) {
    std::ranges::transform(context, context.begin(), ::tolower);
    logs << std::format("{} {}\n", style::ansi("|", {style::Style::Dim}), context);
  }
  void show(std::string_view value) {
    logs << value;
  }
  template <typename T> requires std::is_arithmetic_v<T>
  void show(const T &value) {
    logs << style::ansi(std::format("{}",value),{style::Style::Magenta});
  }
  template <typename T>
  void show(const std::span<T> value) {
    show<
    // show("[");
    // for (std::string delimiter = ""; auto &&x : value)
    //   show(delimiter), show(x), delimiter = ", ";
    // show("]");
  }
  template <std::ranges::range R>
  void show(const R &rng, std::string_view opening = "{", std::string_view closing = "}", std::string_view delimiter = ", ") {
    show(opening);
    for (std::string delim = ""; auto &&x : rng)
      show(delim), show(x), delim = delimiter;
    show(closing);
  }
  template <typename... Args>
  void show(const std::tuple<Args...> value) {
    show("(");
    std::apply([&](Args... x) { ((show(x), show(", ")), ...); }, value);
    show(")");
  }
};
} // namespace rich
