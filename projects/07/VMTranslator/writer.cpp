#ifndef VM_WRITER
#define VM_WRITER

#include <fstream>
#include <iostream>
#include <string>

class Writer {
public:
  static void writeFile(std::string file_contents, std::string path) {
    std::ofstream outputFile(path);

    if (!outputFile.is_open()) {
      std::cerr << "Error opening file!" << std::endl;
    }

    outputFile << file_contents;

    outputFile.close();
  }
};

#endif
