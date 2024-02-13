#include <iomanip>
#include <iostream>
#include <iterator>
#include <ranges>

template <std::ranges::sized_range R>
class track_view : public std::ranges::view_interface<track_view<R>> {
private:
  using TIter = std::ranges::iterator_t<R>;

  struct iterator {
    using value_type = typename std::iterator_traits<TIter>::difference_type;
    using iterator_category = typename std::iterator_traits<TIter>::iterator_category;
    using reference = typename std::iterator_traits<TIter>::reference;
    using difference_type = typename std::iterator_traits<TIter>::difference_type;
    std::string_view done_char = "━";
    std::string_view todo_char = " ";
    std::string_view opening_char = "[";
    std::string_view closing_char = "]";
    std::string_view description;
  
  private:
    TIter iter;
    difference_type length;
    difference_type num_step;
  static constexpr size_t width = 50;

  public:
    iterator(const TIter &iter, difference_type length) : iter(iter), length(length){};
    difference_type count() const {return length;}

    void update() {
      size_t percentage = (num_step - length)*100 / num_step;
      std::clog << opening_char;
      for (size_t i = 0; i < width; i++)
        std::clog << (i * 100 < percentage * width ? done_char : todo_char);
      std::clog << closing_char;
      std::clog << ' ' << description;
      std::clog << percentage << "%";
      std::clog << (count() == 0 ? '\n' : '\r');
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
  track_view(R&& rng):base(std::forward<R>(rng)){};
  auto begin() { return iterator{std::ranges::begin(base), std::ranges::size(base)}; }
  auto end() { return std::default_sentinel; }

private:
  R base;
};

// // here
// template <std::ranges::view V>
// class progress_view: public std::ranges::view_interface<progress_view<V>>
// {
// public:
//   auto begin() const {
//     return progress_indicator_iterator(std::ranges::begin(base));
//   }
//   constexpr auto end() const noexcept { return std::default_sentinel;}
// private:
//   V base;
// };

// template <std::input_or_output_iterator I>
// struct progress_indicator_iterator {
//   static constexpr size_t width = 50;
//   std::string_view done_char = "━";
//   std::string_view todo_char = " ";
//   std::string_view opening_char = "[";
//   std::string_view closing_char = "]";
//   std::string_view description;
// public:
//   using self_type         = progress_indicator_iterator<I>;
//   using iterator_type     = I;
//   using value_type        = typename std::iter_value_t<I>;
//   using difference_type   = typename std::iter_difference_t<I>;
//   using iterator_category = typename I::iterator_category;
//   progress_indicator_iterator(I &&iter, difference_type n) : iter(std::forward<I>(iter)), length(n), num_step(n) {}
//   constexpr difference_type count() const noexcept { return length;}
//   constexpr const iterator_type& base() const & noexcept { return iter;}
//   constexpr iterator_type base() && {return iter;}
//   void update() {
//     size_t percentage = (num_step-length)/num_step;
//     std::clog << opening_char;
//     for (size_t i = 0; i < width; i++)
//       std::clog << (i * 100 < percentage * width ? done_char : todo_char);
//     std::clog << closing_char;
//     std::clog << ' ' << description;
//     std::clog << percentage << "%";
//     std::clog << (count()==0 ? '\n' : '\r');
//   }
//   constexpr decltype(auto) operator*() {
//     return *iter;
//   };
//   constexpr self_type &operator++() {
//     ++iter;
//     --length;
//     update();
//     return *this;
//   }
//   constexpr decltype(auto) operator++(int) {
//     --length;
//     try {
//       update();
//       return iter++;
//     } catch (...) {
//       ++length;
//       throw;
//     }
//   }
//   friend constexpr auto operator<=>(const self_type& a, const self_type &b)
//   {
//     return a.count()<=>b.count();
//   }
//   friend constexpr bool operator==(const self_type &a, std::default_sentinel_t)
//   {
//     return a.count()==0;
//   }

// private:
//   iterator_type iter;
//   difference_type length;
//   const difference_type num_step;
// };

// struct track : std::ranges::range_adaptor_closure<track> {
//   template <std::ranges::range R>
//   constexpr auto operator()(R &&rng) const {
//     return std::ranges::subrange(progress_indicator_iterator(std::ranges::begin(std::forward<I>(rng)), std::ranges::size(rng)), std::default_sentinel);
//   }
// };

struct track {
  template <std::ranges::range R>
  friend constexpr auto operator|(R &&rng, track) {
    return track_view<R>(std::forward<R>(rng));
  }
};
