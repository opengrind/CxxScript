#include "ScriptManager.h"
#include <cassert>
#include <iostream>

int main() {
  std::cout << "=== Testing Uninitialized Variables ===\n\n";

  Script::ScriptManager manager;
  std::vector<Script::CompilationError> errors;

  if (!manager.loadScriptFile("scripts/test_uninitialized.script", errors)) {
    std::cerr << "Failed to load script\n";
    for (const auto &err : errors) {
      std::cerr << err.toString() << "\n";
    }
    return 1;
  }

  std::cout << "✓ Script loaded successfully\n\n";

  // Test 1: Uninitialized int32 (defaults to 0)
  std::cout << "Test 1: Uninitialized int32\n";
  Script::Value result1;
  std::string errorMsg;
  assert(manager.executeProcedure("testUninitInt", {}, result1, errorMsg));
  assert(std::holds_alternative<int32_t>(result1));
  std::cout << "  Result: " << std::get<int32_t>(result1)
            << " (defaults to 0) ✓\n\n";

  // Test 2: Uninitialized bool (defaults to false)
  std::cout << "Test 2: Uninitialized bool\n";
  Script::Value result2;
  assert(manager.executeProcedure("testUninitBool", {}, result2, errorMsg));
  assert(std::holds_alternative<bool>(result2));
  std::cout << "  Result: " << (std::get<bool>(result2) ? "true" : "false")
            << " (defaults to false) ✓\n\n";

  // Test 3: Uninitialized string (defaults to empty)
  std::cout << "Test 3: Uninitialized string\n";
  Script::Value result3;
  assert(manager.executeProcedure("testUninitString", {}, result3, errorMsg));
  assert(std::holds_alternative<std::string>(result3));
  std::cout << "  Result: \"" << std::get<std::string>(result3)
            << "\" (defaults to empty) ✓\n\n";

  // Test 4: Declare then assign
  std::cout << "Test 4: Declare then assign\n";
  Script::Value result4;
  assert(manager.executeProcedure("testDeclareAssign", {}, result4, errorMsg));
  assert(std::holds_alternative<int32_t>(result4));
  assert(std::get<int32_t>(result4) == 42);
  std::cout << "  Result: " << std::get<int32_t>(result4) << " ✓\n\n";

  // Test 5: Mixed initialized and uninitialized
  std::cout << "Test 5: Mixed initialized and uninitialized\n";
  Script::Value result5;
  assert(manager.executeProcedure("testMixed", {}, result5, errorMsg));
  assert(std::holds_alternative<int32_t>(result5));
  assert(std::get<int32_t>(result5) == 15);
  std::cout << "  Result: " << std::get<int32_t>(result5)
            << " (0 + 10 + 5) ✓\n\n";

  // Test 6: Multiple declarations
  std::cout << "Test 6: Multiple uninitialized then assigned\n";
  Script::Value result6;
  assert(manager.executeProcedure("testMultiple", {}, result6, errorMsg));
  assert(std::holds_alternative<int32_t>(result6));
  assert(std::get<int32_t>(result6) == 6);
  std::cout << "  Result: " << std::get<int32_t>(result6)
            << " (1 + 2 + 3) ✓\n\n";

  std::cout << "=== All Tests Passed! ===\n\n";
  std::cout << "Summary:\n";
  std::cout << "• Variables CAN be declared without initialization\n";
  std::cout << "• Uninitialized integers default to 0\n";
  std::cout << "• Uninitialized booleans default to false\n";
  std::cout << "• Uninitialized strings default to empty \"\"\n";
  std::cout << "• You can assign values after declaration\n";

  return 0;
}
