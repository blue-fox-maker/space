#include "../libs/rich.hpp"
#include <format>
#include <source_location>
#include <thread>
#include <vector>

int main(){
  using namespace std::literals;
  std::vector<std::tuple<int>> a = {1,5,3,4,4,8,5,3,4,4,8,5,3,4,4,8,5,3,4,4,8,5,3};
  rich::console.render("# test");
  rich::console.render();
  rich::console.render("the first arg is {} and the second one {}", 7.0, "here"s);
  rich::console.render();
  rich::console.render("- we can use this to make a list");
  rich::console.render("- we can use this to make a list {}", 2);
  rich::console.render("- what about character {}", 'h');
  rich::console.render();
  rich::console.render("should i write rich::console every single time?");
  rich::console.render();
  rich::console.render("but if there is a line break between every line");
  rich::console.render("but if there is a line break between every line");
  rich::console.render("but if there is a line break between every line");
  rich::console.render("but if there is a line break between every line");
  rich::console.render("but if there is a line break between every line");
  rich::console.render("but if there is a line break between every line");
  rich::console.render("but if there is a line break between every line");
  rich::console.render();
  auto [x,y] = rich::console.read<std::pair<int,std::string>>("> input a integer and a string");
  rich::console.render("the input is {} and {}",x,y);
  rich::console.render();
  rich::console.render("it may look bad");
  rich::console.render();
  rich::console.render("> when in rome, do as romans do");
  rich::console.render();
  rich::console.render("the bench of sleep");
  rich::console.bench([](auto x){ 
    auto a = x;
    std::this_thread::sleep_for(0.1s);
  },a);
  rich::console.render();
  rich::console.render("another question arised");
  rich::console.render();
  rich::console.render("is it convinient for us to do something like this?");
}
