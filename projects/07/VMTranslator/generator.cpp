#ifndef VM_GENERATOR
#define VM_GENERATOR

#include "parser.cpp"
#include <sstream>

class Generator {
private:
  int count = 0;

  std::string processAdd(Command command) {
    return "@SP\nAM=M-1\nD=M\nA=A-1\nM=M+D\n";
  };
  std::string processSub(Command command) {
    return "@SP\nAM=M-1\nD=M\nA=A-1\nM=M-D\n";
  };
  std::string processAnd(Command command) {
    return "@SP\nAM=M-1\nD=M\nA=A-1\nM=M&D\n";
  };
  std::string processOr(Command command) {
    return "@SP\nAM=M-1\nD=M\nA=A-1\nM=M|D\n";
  };
  std::string processNeg(Command command) {
    return "@SP\nAM=M-1\nM=-M\n@SP\nM=M+1\n";
  };
  std::string processNot(Command command) {
    return "@SP\nAM=M-1\nM=!M\n@SP\nM=M+1\n";
  };
  std::string processEq(Command command) {
    std::stringstream ss;
    ss << "@SP\nAM=M-1\nD=M\nA=A-1\nD=M-D\n@TRUE" << count << "\nD;JEQ\n"
       << "@SP\nAM=M-1\nM=0\n@SP\nM=M1\n@CONTINUE" << count << "\n0;JMP\n(TRUE"
       << count << ")\n@SP\nAM=M-1\nM=-1\n@SP\nM=M1\n(CONTINUE" << count
       << ")\n";
    count = count + 1;
    return ss.str();
  };
  std::string processGt(Command command) {
    std::stringstream ss;
    ss << "@SP\nAM=M-1\nD=M\nA=A-1\nD=M-D\n@TRUE" << count << "\nD;JGT\n"
       << "@SP\nAM=M-1\nM=0\n@SP\nM=M1\n@CONTINUE" << count << "\n0;JMP\n(TRUE"
       << count << ")\n@SP\nAM=M-1\nM=-1\n@SP\nM=M1\n(CONTINUE" << count
       << ")\n";
    count = count + 1;
    return ss.str();
  };
  std::string processLt(Command command) {
    std::stringstream ss;
    ss << "@SP\nAM=M-1\nD=M\nA=A-1\nD=M-D\n@TRUE" << count << "\nD;JLT\n"
       << "@SP\nAM=M-1\nM=0\n@SP\nM=M1\n@CONTINUE" << count << "\n0;JMP\n(TRUE"
       << count << ")\n@SP\nAM=M-1\nM=-1\n@SP\nM=M1\n(CONTINUE" << count
       << ")\n";
    count = count + 1;
    return ss.str();
  };

  std::string processArithmeticCommand(Command command) {
    switch (command.getArithmeticOperation()) {
    case ADD:
      return processAdd(command);
    case SUB:
      return processSub(command);
    case OR:
      return processOr(command);
    case AND:
      return processAnd(command);
    case NOT:
      return processNot(command);
    case EQ:
      return processEq(command);
    case GT:
      return processGt(command);
    case LT:
      return processLt(command);
    case NEG:
      return processNeg(command);
    }
  };
  std::string processMemorySegmentAccessCommand(Command command){
      //TODO
      return "";
  };

public:
  std::string processCommand(Command command) {
    if (command.getCommandType() == ARITHMETIC_OPERATION) {
      return processArithmeticCommand(command);
    }
    return processMemorySegmentAccessCommand(command);
  };
};

#endif