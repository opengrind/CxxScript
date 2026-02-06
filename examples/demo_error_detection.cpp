#include "ScriptManager.h"
#include <iostream>

using namespace Script;

int main() {
  std::cout << "\n=== Testing Various Unexpected Characters ===" << std::endl;

  ScriptManager manager;
  std::vector<CompilationError> errors;

  bool success = manager.checkScript("test_unexpected_chars.script", errors);

  std::cout << "\nCompilation Result: " << (success ? "SUCCESS" : "FAILED")
            << std::endl;
  std::cout << "Total errors found: " << errors.size() << "\n" << std::endl;

  if (!errors.empty()) {
    for (size_t i = 0; i < errors.size(); ++i) {
      std::cout << "Error " << (i + 1) << ": " << errors[i].toString()
                << std::endl;
    }
  }

  return success ? 0 : 1;
}
