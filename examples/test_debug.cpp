#include "ScriptManager.h"
#include <iostream>

using namespace Script;

int main() {
  ScriptManager manager;
  std::vector<CompilationError> errors;

  std::string source = R"(
    string concat7() {
      string prefix = "Values: ";
      int8 a = 127;
      string sep1 = ", ";
      uint16 b = 65535;
      string sep2 = ", ";
      int64 c = 9223372036854775807;
      return prefix + a + sep1 + b + sep2 + c;
    }
  )";
  
  manager.loadScriptSource(source, "debug.script", errors);
  
  if (!errors.empty()) {
    for (const auto& err : errors) {
      std::cout << "Error: " << err.toString() << std::endl;
    }
    return 1;
  }

  std::string errorMsg;
  Value result;
  std::vector<Value> noArgs;
  bool success = manager.executeProcedure("concat7", noArgs, result, errorMsg);
  
  if (!success) {
    std::cout << "Execution error: " << errorMsg << std::endl;
    return 1;
  }
  
  std::string resultStr = std::get<std::string>(result);
  std::cout << "Result: [" << resultStr << "]" << std::endl;
  std::cout << "Length: " << resultStr.length() << std::endl;
  
  return 0;
}
