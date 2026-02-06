#include "ScriptManager.h"
#include <iostream>

using namespace Script;

int main() {
  ScriptManager manager;
  std::vector<CompilationError> errors;

  // Test current escape support
  std::string source = R"(
    string testEscape() {
      string s = "Hello \"World\"";
      return s;
    }
  )";
  
  if (!manager.loadScriptSource(source, "test.script", errors)) {
    std::cout << "Compilation errors:" << std::endl;
    for (const auto& err : errors) {
      std::cout << "  " << err.toString() << std::endl;
    }
    return 1;
  }

  Value result;
  std::string errorMsg;
  std::vector<Value> args;
  
  if (manager.executeProcedure("testEscape", args, result, errorMsg)) {
    std::string str = std::get<std::string>(result);
    std::cout << "Result: [" << str << "]" << std::endl;
  } else {
    std::cout << "Error: " << errorMsg << std::endl;
  }
  
  return 0;
}
