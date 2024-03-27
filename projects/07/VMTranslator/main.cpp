#include "generator.cpp"

int main() {
  std::string test_string = "add";

  Command myCommand(test_string);
  Generator generator;

  std::string output = generator.processCommand(myCommand);
  std::cout << output;

  return 0;
}
