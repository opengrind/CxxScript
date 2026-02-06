#include "ScriptManager.h"
#include <cassert>
#include <iostream>

int main() {
  std::cout << "=== Testing Whitespace Handling ===\n\n";

  Script::ScriptManager manager;
  std::vector<Script::CompilationError> errors;

  // Load script with various whitespace patterns
  if (!manager.loadScriptFile("scripts/test_whitespace.script", errors)) {
    std::cerr << "Failed to load test_whitespace.script\n";
    for (const auto &err : errors) {
      std::cerr << err.toString() << "\n";
    }
    return 1;
  }

  std::cout
      << "✓ Script loaded successfully (all whitespace patterns parsed)\n\n";

  // Test 1: No indentation
  std::cout << "Test 1: No indentation\n";
  Script::Value result1;
  std::string errorMsg;
  assert(manager.executeProcedure("noIndent", {}, result1, errorMsg));
  assert(std::holds_alternative<int32_t>(result1));
  assert(std::get<int32_t>(result1) == 42);
  std::cout << "  Result: " << std::get<int32_t>(result1) << " ✓\n\n";

  // Test 2: With indentation
  std::cout << "Test 2: With indentation (2 spaces)\n";
  Script::Value result2;
  assert(manager.executeProcedure("withIndent", {}, result2, errorMsg));
  assert(std::holds_alternative<int32_t>(result2));
  assert(std::get<int32_t>(result2) == 42);
  std::cout << "  Result: " << std::get<int32_t>(result2) << " ✓\n\n";

  // Test 3: With tabs
  std::cout << "Test 3: With tabs\n";
  Script::Value result3;
  assert(manager.executeProcedure("withTabs", {}, result3, errorMsg));
  assert(std::holds_alternative<int32_t>(result3));
  assert(std::get<int32_t>(result3) == 42);
  std::cout << "  Result: " << std::get<int32_t>(result3) << " ✓\n\n";

  // Test 4: Mixed indentation
  std::cout << "Test 4: Mixed indentation (spaces and tabs)\n";
  Script::Value result4;
  assert(manager.executeProcedure("mixedIndent", {}, result4, errorMsg));
  assert(std::holds_alternative<int32_t>(result4));
  assert(std::get<int32_t>(result4) == 30);
  std::cout << "  Result: " << std::get<int32_t>(result4) << " ✓\n\n";

  // Test 5: Multiline with varying indentation
  std::cout << "Test 5: Multiline with varying indentation\n";
  Script::Value result5;
  assert(manager.executeProcedure("multiline", {}, result5, errorMsg));
  assert(std::holds_alternative<int32_t>(result5));
  assert(std::get<int32_t>(result5) == 3);
  std::cout << "  Result: " << std::get<int32_t>(result5) << " ✓\n\n";

  // Test 6: Lots of spaces between tokens
  std::cout << "Test 6: Lots of spaces between tokens\n";
  Script::Value result6;
  assert(manager.executeProcedure("lotsOfSpaces", {}, result6, errorMsg));
  assert(std::holds_alternative<int32_t>(result6));
  assert(std::get<int32_t>(result6) == 16);
  std::cout << "  Result: " << std::get<int32_t>(result6) << " ✓\n\n";

  // Test 7: Single line procedure
  std::cout << "Test 7: Single line procedure\n";
  Script::Value result7;
  assert(manager.executeProcedure("oneLine", {}, result7, errorMsg));
  assert(std::holds_alternative<int32_t>(result7));
  assert(std::get<int32_t>(result7) == 99);
  std::cout << "  Result: " << std::get<int32_t>(result7) << " ✓\n\n";

  // Test 8: Single line with logic
  std::cout << "Test 8: Single line with multiple statements\n";
  Script::Value result8;
  assert(manager.executeProcedure("oneLineWithLogic", {}, result8, errorMsg));
  assert(std::holds_alternative<int32_t>(result8));
  assert(std::get<int32_t>(result8) == 30);
  std::cout << "  Result: " << std::get<int32_t>(result8) << " ✓\n\n";

  std::cout << "=== All Whitespace Tests Passed! ===\n";
  std::cout << "\nConclusion:\n";
  std::cout << "• Indentation does NOT matter\n";
  std::cout << "• Spaces and tabs are treated equally as whitespace\n";
  std::cout << "• Multiline formatting is flexible\n";
  std::cout << "• Any amount of whitespace between tokens is allowed\n";
  std::cout << "• Procedures can be written on a single line\n";
  std::cout << "• Whitespace is only significant inside string literals\n";

  return 0;
}
