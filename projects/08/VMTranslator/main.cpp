#include "generator.cpp"
#include "writer.cpp"
#include <filesystem>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    throw std::runtime_error("Most pass at least and only 1 argument.");
  }
  std::filesystem::path path = argv[1];

  int16_t jump_count = 0;
  int16_t call_count = 1;

  if (std::filesystem::exists(path)) {
    if (std::filesystem::is_regular_file(path)) {
      Generator generator(path.filename(), 0, 0);
      std::vector<Command> command_list = CommandList::parseFile(path);
      GeneratorReturn generator_return =
          generator.processCommandList(command_list);

      path.replace_extension(".asm");
      Writer::writeFile(generator_return.asm_text, path);
    } else if (std::filesystem::is_directory(path)) {
      std::stringstream file_contents;

      file_contents << Generator::processSysInitCommand();

      for (const auto &entry : std::filesystem::directory_iterator(path)) {
        if (entry.path().extension() == ".vm") {
          Generator generator(entry.path().filename(), jump_count, call_count);
          std::vector<Command> command_list =
              CommandList::parseFile(entry.path());

          GeneratorReturn generator_return =
              generator.processCommandList(command_list);

          jump_count = generator_return.jump_count;
          call_count = generator_return.call_count;

          file_contents << generator_return.asm_text;
        }
      }

      path.replace_extension(".asm");
      Writer::writeFile(file_contents.str(), path);
    }
  } else {
    std::cerr << "The path does not exist." << std::endl;
  }

  return 0;
}
