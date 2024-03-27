#include "generator.cpp"
#include "writer.cpp"
#include <filesystem>

int main() {
  Generator generator;
  std::filesystem::path path = "add.vm";

  std::vector<Command> command_list = CommandList::parseFile(path);
  std::string file_contents =
      generator.processCommandList(command_list, path.filename());

  path.replace_extension(".asm");
  Writer::writeFile(file_contents, path);

  return 0;
}
