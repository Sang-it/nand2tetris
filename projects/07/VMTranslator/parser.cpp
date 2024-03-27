#ifndef VM_PARSER
#define VM_PARSER

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
enum CommandType { MEMORY_SEGMENT, ARITHMETIC_OPERATION };

class Command {
private:
  std::optional<int16_t> offset;
  std::optional<MemorySegment> memory_segment;
  std::optional<MemorySegmentAccessType> access_type;
  std::optional<ArithmeticOperation> arithmetic_operation;
  CommandType command_type;

  std::vector<std::string> splitString(const std::string &input, char delim) {
    std::istringstream string_stream(input);
    std::vector<std::string> tokens;
    std::string token;

    while (std::getline(string_stream, token, delim)) {
      tokens.push_back(token);
    }

    return tokens;
  };

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
      throw std::invalid_argument("Invalid string for MemorySegment enum");
    }
  }

public:
  Command(std::string line) {
    std::vector<std::string> splits = splitString(line, ' ');
    if (splits.size() == 1) {
      command_type = ARITHMETIC_OPERATION;
      arithmetic_operation = getOperationKind(splits[0]);
    } else if (splits.size() == 3) {
      command_type = MEMORY_SEGMENT;
      access_type = getAccessKind(splits[0]);
      memory_segment = getMemorySegmentKind(splits[1]);
      offset = std::stoi(splits[2]);
    } else {
      throw std::invalid_argument("Invalid string for Command");
    }
  };

  CommandType getCommandType() { return command_type; };
  ArithmeticOperation getArithmeticOperation() {
    return arithmetic_operation.value();
  };
};

#endif
