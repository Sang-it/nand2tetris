#ifndef VM_PARSER
#define VM_PARSER

#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

enum MemorySegment {
  CONSTANT,
  STATIC,
  POINTER,
  LOCAL,
  ARGUMENT,
  THIS,
  THAT,
  TEMP
};
enum ArithmeticOperation { ADD, SUB, NEG, EQ, GT, LT, AND, OR, NOT };
enum MemorySegmentAccessType { PUSH, POP };
enum BranchType { LABEL, IF_GOTO, GOTO };
enum CommandType {
  MEMORY_SEGMENT,
  ARITHMETIC_OPERATION,
  BRANCH_COMMAND,
  FUNCTION,
  RETURN,
  CALL
};

class Command {
private:
  std::optional<int16_t> offset;
  std::optional<MemorySegment> memory_segment;
  std::optional<MemorySegmentAccessType> access_type;

  std::optional<ArithmeticOperation> arithmetic_operation;

  std::optional<BranchType> branch_type;
  std::optional<std::string> label;

  std::optional<std::string> function_name;
  std::optional<int16_t> number_of_local_variables;
  std::optional<int16_t> number_of_args;

  CommandType command_type;

  std::string removeComments(const std::string &line) {
    size_t pos = line.find("//");
    if (pos != std::string::npos) {
      return line.substr(0, pos);
    }
    return line;
  }

  std::vector<std::string> splitBySpace(const std::string &line) {
    std::vector<std::string> tokens;
    std::istringstream iss(line);
    std::string token;
    while (iss >> token) {
      tokens.push_back(token);
    }
    return tokens;
  }

  ArithmeticOperation getOperationKind(std::string operation_string) {
    static const std::unordered_map<std::string, ArithmeticOperation>
        operation_map = {
            {"add", ADD}, {"and", AND}, {"or", OR}, {"sub", SUB}, {"neg", NEG},
            {"eq", EQ},   {"gt", GT},   {"lt", LT}, {"not", NOT},
        };

    auto item = operation_map.find(operation_string);
    if (item != operation_map.end()) {
      return item->second;
    } else {
      throw std::invalid_argument(
          "Invalid string for ArithmeticOperation enum");
    }
  }

  MemorySegment getMemorySegmentKind(std::string segment_string) {
    static const std::unordered_map<std::string, MemorySegment>
        memory_segment_map = {
            {"local", LOCAL},   {"argument", ARGUMENT}, {"constant", CONSTANT},
            {"this", THIS},     {"that", THAT},         {"temp", TEMP},
            {"static", STATIC}, {"pointer", POINTER},
        };

    auto item = memory_segment_map.find(segment_string);
    if (item != memory_segment_map.end()) {
      return item->second;
    } else {
      throw std::invalid_argument("Invalid string for MemorySegment enum");
    }
  }

  MemorySegmentAccessType getAccessKind(std::string access_string) {
    static const std::unordered_map<std::string, MemorySegmentAccessType>
        access_map = {
            {"push", PUSH},
            {"pop", POP},
        };

    auto item = access_map.find(access_string);
    if (item != access_map.end()) {
      return item->second;
    } else {
      throw std::invalid_argument(
          "Invalid string for MemorySegmentAccessType enum");
    }
  }

  BranchType getBranchKind(std::string access_string) {
    static const std::unordered_map<std::string, BranchType> branch_map = {
        {"label", LABEL},
        {"if-goto", IF_GOTO},
        {"goto", GOTO},
    };

    auto item = branch_map.find(access_string);

    if (item != branch_map.end()) {
      return item->second;
    } else {
      throw std::invalid_argument("Invalid string for BranchCommand enum");
    }
  }

public:
  Command(std::string line) {
    std::vector<std::string> splits = splitBySpace(removeComments(line));
    if (splits.size() == 1) {
      if (splits[0] == "return") {
        this->command_type = RETURN;
      } else {
        this->command_type = ARITHMETIC_OPERATION;
        this->arithmetic_operation = getOperationKind(splits[0]);
      }
    } else if (splits.size() == 2) {
      this->command_type = BRANCH_COMMAND;
      this->branch_type = getBranchKind(splits[0]);
      this->label = splits[1];
    } else if (splits.size() == 3) {
      if (splits[0] == "function") {
        this->command_type = FUNCTION;
        this->function_name = splits[1];
        this->number_of_local_variables = std::stoi(splits[2]);
      } else if (splits[0] == "call") {
        this->command_type = CALL;
        this->function_name = splits[1];
        this->number_of_args = std::stoi(splits[2]);
      } else {
        this->command_type = MEMORY_SEGMENT;
        this->access_type = getAccessKind(splits[0]);
        this->memory_segment = getMemorySegmentKind(splits[1]);
        this->offset = std::stoi(splits[2]);
      }

    } else {
      throw std::invalid_argument("Invalid string for Command");
    }
  };

  CommandType getCommandType() { return this->command_type; };

  ArithmeticOperation getArithmeticOperation() {
    return this->arithmetic_operation.value();
  };

  int16_t getOffset() { return this->offset.value(); };

  MemorySegment getMemorySegment() { return this->memory_segment.value(); }

  MemorySegmentAccessType getAccessType() { return this->access_type.value(); }

  BranchType getBranchType() { return this->branch_type.value(); }

  std::string getLabel() { return this->label.value(); }

  std::string getFunctionName() { return this->function_name.value(); }

  int16_t getNumberOfLocalVars() {
    return this->number_of_local_variables.value();
  }

  int16_t getNumberOfArgs() { return this->number_of_args.value(); }
};

class CommandList {
public:
  static std::string trim(const std::string &str) {
    size_t start = str.find_first_not_of(" \t\n\r\f\v");
    size_t end = str.find_last_not_of(" \t\n\r\f\v");
    if (start == std::string::npos) {
      return "";
    }
    return str.substr(start, end - start + 1);
  }

  static bool isValidLine(std::string line) {
    if (line.empty()) {
      return false;
    }

    std::string comment_start = "//";
    if (!strncmp(line.c_str(), comment_start.c_str(), comment_start.size())) {
      return false;
    }

    return true;
  }

  static std::vector<Command> parseFile(std::string path) {
    std::vector<Command> command_list;

    std::ifstream input_file(path);
    if (!input_file.is_open()) {
      std::cerr << "Error opening file!" << std::endl;
    }

    std::string line;
    while (std::getline(input_file, line)) {
      if (isValidLine(trim(line))) {
        Command command(line);
        command_list.push_back(command);
      }
    }

    input_file.close();
    return command_list;
  };
};

#endif
