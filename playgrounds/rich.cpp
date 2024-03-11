#include "../libs/rich.hpp"
#include <format>
#include <source_location>
#include <thread>
#include <vector>

int main(){
  using namespace std::literals;
  std::vector<std::tuple<int>> a = {1,5,3,4,4,8,5,3,4,4,8,5,3,4,4,8,5,3,4,4,8,5,3};
  rich::console.println("# test");
  rich::console.println();
  rich::console.println("the first arg is {} and the second one {}", 7.0, "here"s);
  rich::console.println();
  rich::console.println("- we can use this to make a list");
  rich::console.println("- we can use this to make a list {}", 2);
  rich::console.println("- what about character {}", 'h');
  rich::console.println();
  rich::console.println("should i write rich::console every single time?");
  rich::console.println();
  rich::console.println("but if there is a line break between every line");
  rich::console.println("but if there is a line break between every line");
  rich::console.println("but if there is a line break between every line");
  rich::console.println("but if there is a line break between every line");
  rich::console.println("but if there is a line break between every line");
  rich::console.println("but if there is a line break between every line");
  rich::console.println("but if there is a line break between every line");
  rich::console.println();
  auto [x,y] = rich::console.read<std::pair<int,std::string>>("> input a integer and a string");
  rich::console.println("the input is {} and {}",x,y);
  rich::console.println();
  rich::console.println("it may look bad");
  rich::console.println();
  rich::console.println("> when in rome, do as romans do");
  rich::console.println();
  rich::console.println("the bench of sleep");
  rich::console.bench([](auto x){ 
    auto a = x;
    std::this_thread::sleep_for(0.1s);
  },a);
  rich::console.println();
  rich::console.println("another question arised");
  rich::console.println();
  rich::console.println("is it convinient for us to do something like this?");
}
