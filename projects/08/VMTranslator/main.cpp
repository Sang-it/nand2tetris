#include "generator.cpp"
#include "writer.cpp"
#include <filesystem>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    throw std::runtime_error("Most pass at least and only 1 argument.");
  }
  std::filesystem::path path = argv[1];
  Generator generator(path.filename());
  std::vector<Command> command_list = CommandList::parseFile(path);
  std::string file_contents =
      generator.processCommandList(command_list, path.filename());

  path.replace_extension(".asm");
  Writer::writeFile(file_contents, path);

  return 0;
}
