#ifndef VM_GENERATOR
#define VM_GENERATOR

#include "parser.cpp"
#include <sstream>

struct GeneratorReturn {
  std::string asm_text;
  int16_t jump_count;
  int16_t call_count;
};

class Generator {
private:
  std::string filename;
  int16_t jump_count;
  int16_t call_count;

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
    ss << "@SP\nAM=M-1\nD=M\nA=A-1\nD=M-D\n@TRUE" << jump_count << "\nD;JEQ\n"
       << "@SP\nAM=M-1\nM=0\n@SP\nM=M+1\n@CONTINUE" << jump_count
       << "\n0;JMP\n(TRUE" << jump_count
       << ")\n@SP\nAM=M-1\nM=-1\n@SP\nM=M+1\n(CONTINUE" << jump_count++
       << ")\n";
    return ss.str();
  };
  std::string processGt(Command command) {
    std::stringstream ss;
    ss << "@SP\nAM=M-1\nD=M\nA=A-1\nD=M-D\n@TRUE" << jump_count << "\nD;JGT\n"
       << "@SP\nAM=M-1\nM=0\n@SP\nM=M+1\n@CONTINUE" << jump_count
       << "\n0;JMP\n(TRUE" << jump_count
       << ")\n@SP\nAM=M-1\nM=-1\n@SP\nM=M+1\n(CONTINUE" << jump_count++
       << ")\n";
    return ss.str();
  };
  std::string processLt(Command command) {

    std::stringstream ss;
    ss << "@SP\nAM=M-1\nD=M\nA=A-1\nD=M-D\n@TRUE" << jump_count << "\nD;JLT\n"
       << "@SP\nAM=M-1\nM=0\n@SP\nM=M+1\n@CONTINUE" << jump_count
       << "\n0;JMP\n(TRUE" << jump_count
       << ")\n@SP\nAM=M-1\nM=-1\n@SP\nM=M+1\n(CONTINUE" << jump_count++
       << ")\n";
    return ss.str();
  };

  std::string processConstant(Command command) {
    std::stringstream ss;
    ss << "@" << command.getOffset() << "\nD=A\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
    return ss.str();
  }
  std::string processStaticPush(Command command) {
    std::stringstream ss;
    ss << "@" << this->filename << "." << command.getOffset()
       << "\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
    return ss.str();
  }
  std::string processPointerPush(Command command) {
    std::stringstream ss;
    std::string memory_segment = command.getOffset() ? "THAT" : "THIS";
    ss << "@" << memory_segment << "\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
    return ss.str();
  }
  std::string processTempPush(Command command) {
    std::stringstream ss;
    ss << "@R5\nD=A\n@" << command.getOffset()
       << "\nA=D+A\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
    return ss.str();
  }
  std::string processLocalPush(Command command) {
    std::stringstream ss;
    ss << "@LCL\nD=M\n@" << command.getOffset()
       << "\nA=D+A\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
    return ss.str();
  }
  std::string processArgumentPush(Command command) {
    std::stringstream ss;
    ss << "@ARG\nD=M\n@" << command.getOffset()
       << "\nA=D+A\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
    return ss.str();
  }
  std::string processThisPush(Command command) {
    std::stringstream ss;
    ss << "@THIS\nD=M\n@" << command.getOffset()
       << "\nA=D+A\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
    return ss.str();
  }
  std::string processThatPush(Command command) {
    std::stringstream ss;
    ss << "@THAT\nD=M\n@" << command.getOffset()
       << "\nA=D+A\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
    return ss.str();
  }

  std::string processStaticPop(Command command) {
    std::stringstream ss;
    ss << "@SP\nAM=M-1\nD=M\n@" << this->filename << "." << command.getOffset()
       << "\nM=D\n";
    return ss.str();
  }
  std::string processPointerPop(Command command) {
    std::stringstream ss;
    std::string memory_segment = command.getOffset() ? "THAT" : "THIS";
    ss << "@" << memory_segment
       << "\nD=A\n@R13\nM=D\n@SP\nAM=M-1\nD=M\n@R13\nA=M\nM=D\n";
    return ss.str();
  }
  std::string processTempPop(Command command) {
    std::stringstream ss;
    ss << "@R5\nD=A\n"
       << "@" << command.getOffset()
       << "\nD=D+A\n@R13\nM=D\n@SP\nAM=M-1\nD=M\n@R13\nA=M\nM=D\n";
    return ss.str();
  }
  std::string processLocalPop(Command command) {
    std::stringstream ss;
    ss << "@LCL\nD=M\n"
       << "@" << command.getOffset()
       << "\nD=D+A\n@R13\nM=D\n@SP\nAM=M-1\nD=M\n@R13\nA=M\nM=D\n";
    return ss.str();
  }
  std::string processArgumentPop(Command command) {
    std::stringstream ss;
    ss << "@ARG\nD=M\n"
       << "@" << command.getOffset()
       << "\nD=D+A\n@R13\nM=D\n@SP\nAM=M-1\nD=M\n@R13\nA=M\nM=D\n";
    return ss.str();
  }
  std::string processThisPop(Command command) {
    std::stringstream ss;
    ss << "@THIS\nD=M\n"
       << "@" << command.getOffset()
       << "\nD=D+A\n@R13\nM=D\n@SP\nAM=M-1\nD=M\n@R13\nA=M\nM=D\n";
    return ss.str();
  }
  std::string processThatPop(Command command) {
    std::stringstream ss;
    ss << "@THAT\nD=M\n"
       << "@" << command.getOffset()
       << "\nD=D+A\n@R13\nM=D\n@SP\nAM=M-1\nD=M\n@R13\nA=M\nM=D\n";
    return ss.str();
  }

  std::string processPushCommand(Command command) {
    switch (command.getMemorySegment()) {
    case CONSTANT: {
      return processConstant(command);
    }
    case STATIC: {
      return processStaticPush(command);
    }
    case POINTER: {
      return processPointerPush(command);
    }
    case TEMP: {
      return processTempPush(command);
    }
    case LOCAL: {
      return processLocalPush(command);
    }
    case ARGUMENT: {
      return processArgumentPush(command);
    }
    case THIS: {
      return processThisPush(command);
    }
    case THAT: {
      return processThatPush(command);
    }
    }
  };

  std::string processPopCommand(Command command) {
    switch (command.getMemorySegment()) {
    case CONSTANT: {
      throw std::invalid_argument("Invalid member for pop command.");
    }
    case STATIC: {
      return processStaticPop(command);
    }
    case POINTER: {
      return processPointerPop(command);
    }
    case TEMP: {
      return processTempPop(command);
    }
    case LOCAL: {
      return processLocalPop(command);
    }
    case ARGUMENT: {
      return processArgumentPop(command);
    }
    case THIS: {
      return processThisPop(command);
    }
    case THAT: {
      return processThatPop(command);
    }
    }
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

  std::string processLabelCommand(Command command) {
    std::stringstream ss;
    ss << "(" << command.getLabel() << ")\n";
    return ss.str();
  };

  std::string processGotoCommand(Command command) {
    std::stringstream ss;
    ss << "@" << command.getLabel() << "\n0;JMP\n";
    return ss.str();
  };

  std::string processIfGotoCommand(Command command) {
    std::stringstream ss;
    ss << "@SP\nAM=M-1\nD=M\n@" << command.getLabel() << "\nD;JNE\n";
    return ss.str();
  };

  std::string processMemorySegmentAccessCommand(Command command) {
    if (command.getAccessType() == PUSH) {
      return processPushCommand(command);
    }
    return processPopCommand(command);
  };

  std::string processBranchCommand(Command command) {
    switch (command.getBranchType()) {
    case LABEL: {
      return processLabelCommand(command);
    }
    case GOTO: {
      return processGotoCommand(command);
    }
    case IF_GOTO: {
      return processIfGotoCommand(command);
    }
    }
  };

  std::string processFunctionCommand(Command command) {
    std::stringstream ss;
    ss << "(" << command.getFunctionName() << ")\n";
    int16_t number_of_args = command.getNumberOfLocalVars();

    Command _command("push constant 0");
    while (number_of_args--) {
      ss << processConstant(_command);
    }
    return ss.str();
  }

  std::string processReturnCommand(Command command) {
    std::stringstream ss;
    std::initializer_list<const char *> array = {"THAT", "THIS", "ARG", "LCL"};
    ss << "@LCL\nD=M\n@R13\nM=D\n@5\nA=D-A\nD=M\n@R14\nM=D\n@SP\nAM=M-1\nD=M\n"
          "@ARG\nA=M\nM=D\n@ARG\nD=M+1\n@SP\nM=D\n";
    for (const char *symbol : array) {
      ss << "@R13\nD=M-1\nAM=D\nD=M\n@" << symbol << "\nM=D\n";
    }
    ss << "\n@R14\nA=M\n0;JMP\n";
    return ss.str();
  }

  std::string processCallCommand(Command command) {
    std::stringstream ss;
    std::initializer_list<const char *> array = {"LCL", "ARG", "THIS", "THAT"};

    ss << "@" << command.getFunctionName() << "ReturnAddress" << call_count
       << "\nD=A\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";

    for (const char *item : array) {
      ss << "@" << item << "\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
    }

    ss << "@" << command.getNumberOfArgs()
       << "\nD=A\n@5\nD=D+A\n@SP\nD=M-D\n@ARG\nM=D\n@SP\nD=M\n@LCL\nM=D\n@"
       << command.getFunctionName() << "\n0;JMP\n(" << command.getFunctionName()
       << "ReturnAddress" << call_count++ << ")\n";

    return ss.str();
  }

public:
  Generator(std::string filename, int16_t jump_count, int16_t call_count) {
    this->filename = filename;
    this->jump_count = jump_count;
    this->call_count = call_count;
  };

  std::string processCommand(Command command) {
    switch (command.getCommandType()) {
    case ARITHMETIC_OPERATION: {
      return processArithmeticCommand(command);
    }
    case MEMORY_SEGMENT: {
      return processMemorySegmentAccessCommand(command);
    }
    case BRANCH_COMMAND: {
      return processBranchCommand(command);
    }
    case FUNCTION: {
      return processFunctionCommand(command);
    }
    case RETURN: {
      return processReturnCommand(command);
    }
    case CALL: {
      return processCallCommand(command);
    }
    }
  };

  GeneratorReturn processCommandList(std::vector<Command> command_list) {
    std::stringstream ss;
    for (Command command : command_list) {
      std::string processed_command = processCommand(command);
      ss << processed_command << "\n";
    }

    return {ss.str(), jump_count, call_count};
  };

  static std::string processSysInitCommand() {
    std::stringstream ss;

    Command command("call Sys.init 0");
    std::initializer_list<const char *> array = {"LCL", "ARG", "THIS", "THAT"};

    ss << "@256\nD=A\n@SP\nM=D\n"
       << "@" << command.getFunctionName()
       << "ReturnAddress0\nD=A\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";

    for (const char *item : array) {
      ss << "@" << item << "\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
    }

    ss << "@" << command.getNumberOfArgs()
       << "\nD=A\n@5\nD=D+A\n@SP\nD=M-D\n@ARG\nM=D\n@SP\nD=M\n@LCL\nM=D\n@"
       << command.getFunctionName() << "\n0;JMP\n(" << command.getFunctionName()
       << "ReturnAddress0)\n";

    return ss.str();
  }
};

#endif
