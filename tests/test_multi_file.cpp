#include "ScriptManager.h"
#include <gtest/gtest.h>

using namespace Script;

TEST(MultiFileTest, LoadMultipleFiles) {

  ScriptManager manager;
  std::vector<CompilationError> errors;

  // Load math utilities
  bool success = manager.loadScriptFile("scripts/test_files/math_utils.script", errors);
  if (!success && !errors.empty()) {
    std::cout << "Error loading math_utils: " << errors[0].toString()
              << std::endl;
  }
  EXPECT_TRUE(success);
  EXPECT_TRUE(errors.empty());

  // Load string utilities
  success = manager.loadScriptFile("scripts/test_files/string_utils.script", errors);
  if (!success && !errors.empty()) {
    std::cout << "Error loading string_utils: " << errors[0].toString()
              << std::endl;
  }
  EXPECT_TRUE(success);
  EXPECT_TRUE(errors.empty());

  // Load validators
  success = manager.loadScriptFile("scripts/test_files/validators.script", errors);
  if (!success && !errors.empty()) {
    std::cout << "Error loading validators: " << errors[0].toString()
              << std::endl;
  }
  EXPECT_TRUE(success);
  EXPECT_TRUE(errors.empty());

  // Verify all procedures are loaded
  EXPECT_TRUE(manager.hasProcedure("add"));
  EXPECT_TRUE(manager.hasProcedure("multiply"));
  EXPECT_TRUE(manager.hasProcedure("square"));
  EXPECT_TRUE(manager.hasProcedure("concat"));
  EXPECT_TRUE(manager.hasProcedure("greet"));
  EXPECT_TRUE(manager.hasProcedure("formatNumber"));
  EXPECT_TRUE(manager.hasProcedure("isPositive"));
  EXPECT_TRUE(manager.hasProcedure("isInRange"));
  EXPECT_TRUE(manager.hasProcedure("validateAndCompute"));

  // Get list of all procedures
  auto procNames = manager.getProcedureNames();
  EXPECT_TRUE(procNames.size() == 9);

}

TEST(MultiFileTest, CrossFileProcedureCalls) {

  ScriptManager manager;
  std::vector<CompilationError> errors;

  // Load all files
  manager.loadScriptFile("scripts/test_files/math_utils.script", errors);
  manager.loadScriptFile("scripts/test_files/string_utils.script", errors);
  manager.loadScriptFile("scripts/test_files/main_logic.script", errors);

  std::string errorMsg;
  Value result;

  // Test calling add from main_logic which calls math_utils
  std::vector<Value> args1 = {static_cast<int32_t>(10),
                              static_cast<int32_t>(20)};
  bool success =
      manager.executeProcedure("computeSum", args1, result, errorMsg);
  EXPECT_TRUE(success);
  EXPECT_TRUE(std::get<int32_t>(result) == 30);

  // Test calling square (which calls multiply) from main_logic
  std::vector<Value> args2 = {static_cast<int32_t>(3), static_cast<int32_t>(4)};
  success =
      manager.executeProcedure("computeSquareSum", args2, result, errorMsg);
  EXPECT_TRUE(success);
  EXPECT_TRUE(std::get<int32_t>(result) == 25); // 3^2 + 4^2 = 9 + 16

  // Test string operations across files
  std::vector<Value> args3 = {std::string("John"), std::string("Doe")};
  success = manager.executeProcedure("makeGreeting", args3, result, errorMsg);
  EXPECT_TRUE(success);
  EXPECT_TRUE(std::get<std::string>(result) == "Hello, John Doe");

}

TEST(MultiFileTest, CrossFileWithExternalFunctions) {
  std::cout << "Testing cross-file calls with external C++ functions..."
            << std::endl;

  ScriptManager manager;

  // Register external C++ functions
  manager.registerExternalFunction(
      "toString", [](const std::vector<Value> &args) -> Value {
        int32_t num = std::get<int32_t>(args[0]);
        return std::string("Number: ") + std::to_string(num);
      });

  manager.registerExternalFunction(
      "validate", [](const std::vector<Value> &args) -> Value {
        int32_t x = std::get<int32_t>(args[0]);
        return x * 2; // Simple validation: double the value
      });

  std::vector<CompilationError> errors;

  // Load script files
  manager.loadScriptFile("scripts/test_files/string_utils.script", errors);
  manager.loadScriptFile("scripts/test_files/validators.script", errors);
  manager.loadScriptFile("scripts/test_files/main_logic.script", errors);

  std::string errorMsg;
  Value result;

  // Test formatResult which calls formatNumber which calls external toString
  std::vector<Value> args1 = {static_cast<int32_t>(42)};
  bool success =
      manager.executeProcedure("formatResult", args1, result, errorMsg);
  EXPECT_TRUE(success);
  EXPECT_TRUE(std::get<std::string>(result) == "Number: 42");

  // Test validateAndCompute which calls external validate
  std::vector<Value> args2 = {static_cast<int32_t>(10)};
  success =
      manager.executeProcedure("validateAndCompute", args2, result, errorMsg);
  EXPECT_TRUE(success);
  EXPECT_TRUE(std::get<bool>(result) == true); // validate(10) = 20 > 0

}

TEST(MultiFileTest, ProcedureInfoFromMultipleFiles) {

  ScriptManager manager;
  std::vector<CompilationError> errors;

  manager.loadScriptFile("scripts/test_files/math_utils.script", errors);
  manager.loadScriptFile("scripts/test_files/string_utils.script", errors);

  // Check math_utils procedures
  ScriptManager::ProcedureInfo info;
  EXPECT_TRUE(manager.getProcedureInfo("add", info));
  EXPECT_TRUE(info.name == "add");
  EXPECT_TRUE(info.returnType == DataType::INT32);
  EXPECT_TRUE(info.parameters.size() == 2);
  EXPECT_TRUE(info.filename == "scripts/test_files/math_utils.script");

  // Check string_utils procedures
  EXPECT_TRUE(manager.getProcedureInfo("greet", info));
  EXPECT_TRUE(info.name == "greet");
  EXPECT_TRUE(info.returnType == DataType::STRING);
  EXPECT_TRUE(info.parameters.size() == 1);
  EXPECT_TRUE(info.filename == "scripts/test_files/string_utils.script");

}

TEST(MultiFileTest, DuplicateProcedureNames) {

  // Create two files with same procedure name
  std::string source1 = "int32 duplicate(int32 x) { return x * 2; }";
  std::string source2 = "int32 duplicate(int32 x) { return x * 3; }";

  ScriptManager manager;
  std::vector<CompilationError> errors;

  // Load first file
  bool success = manager.loadScriptSource(source1, "file1.script", errors);
  EXPECT_TRUE(success);

  // Load second file - should succeed and overwrite
  success = manager.loadScriptSource(source2, "file2.script", errors);
  EXPECT_TRUE(success);

  // Execute - should use the last loaded version
  std::vector<Value> args = {static_cast<int32_t>(5)};
  Value result;
  std::string errorMsg;

  success = manager.executeProcedure("duplicate", args, result, errorMsg);
  EXPECT_TRUE(success);
  EXPECT_TRUE(std::get<int32_t>(result) == 15); // Uses file2 version (x * 3)

  std::cout << "  ✓ Duplicate procedure names handled correctly" << std::endl;
}

TEST(MultiFileTest, ClearAndReload) {

  ScriptManager manager;
  std::vector<CompilationError> errors;

  // Load initial files
  manager.loadScriptFile("scripts/test_files/math_utils.script", errors);
  EXPECT_TRUE(manager.hasProcedure("add"));

  // Clear all
  manager.clear();
  EXPECT_TRUE(!manager.hasProcedure("add"));
  EXPECT_TRUE(manager.getProcedureNames().empty());

  // Reload
  manager.loadScriptFile("scripts/test_files/math_utils.script", errors);
  EXPECT_TRUE(manager.hasProcedure("add"));

  std::string errorMsg;
  Value result;
  std::vector<Value> args = {static_cast<int32_t>(5), static_cast<int32_t>(10)};
  bool success = manager.executeProcedure("add", args, result, errorMsg);
  EXPECT_TRUE(success);
  EXPECT_TRUE(std::get<int32_t>(result) == 15);

}

TEST(MultiFileTest, ExternalFunctionPersistenceAcrossFiles) {
  std::cout << "Testing external function persistence across file loads..."
            << std::endl;

  ScriptManager manager;

  // Register external function first
  manager.registerExternalFunction(
      "externalAdd", [](const std::vector<Value> &args) -> Value {
        return std::get<int32_t>(args[0]) + std::get<int32_t>(args[1]);
      });

  // Create script that uses external function
  std::string source1 =
      "int32 useExternal(int32 a, int32 b) { return externalAdd(a, b); }";
  std::string source2 =
      "int32 doubleExternal(int32 a, int32 b) { return externalAdd(a, b) * "
      "2; }";

  std::vector<CompilationError> errors;

  // Load multiple files that use the same external function
  manager.loadScriptSource(source1, "ext1.script", errors);
  manager.loadScriptSource(source2, "ext2.script", errors);

  std::string errorMsg;
  Value result;
  std::vector<Value> args = {static_cast<int32_t>(5), static_cast<int32_t>(3)};

  // Test from first file
  bool success =
      manager.executeProcedure("useExternal", args, result, errorMsg);
  EXPECT_TRUE(success);
  EXPECT_TRUE(std::get<int32_t>(result) == 8);

  // Test from second file
  success = manager.executeProcedure("doubleExternal", args, result, errorMsg);
  EXPECT_TRUE(success);
  EXPECT_TRUE(std::get<int32_t>(result) == 16);

}

TEST(MultiFileTest, ComplexMultiFileScenario) {

  ScriptManager manager;

  // Register multiple external functions
  manager.registerExternalFunction("log",
                                   [](const std::vector<Value> &args) -> Value {
                                     // Simulate logging - just return the value
                                     return args[0];
                                   });

  manager.registerExternalFunction("clamp",
                                   [](const std::vector<Value> &args) -> Value {
                                     int32_t value = std::get<int32_t>(args[0]);
                                     int32_t min = std::get<int32_t>(args[1]);
                                     int32_t max = std::get<int32_t>(args[2]);
                                     if (value < min)
                                       return min;
                                     if (value > max)
                                       return max;
                                     return value;
                                   });

  std::vector<CompilationError> errors;

  // Load all utility files
  manager.loadScriptFile("scripts/test_files/math_utils.script", errors);
  manager.loadScriptFile("scripts/test_files/string_utils.script", errors);
  manager.loadScriptFile("scripts/test_files/validators.script", errors);
  manager.loadScriptFile("scripts/test_files/main_logic.script", errors);

  EXPECT_TRUE(errors.empty());

  std::string errorMsg;
  Value result;

  // Complex scenario: Use procedures from multiple files
  // 1. Validate a number
  std::vector<Value> args1 = {static_cast<int32_t>(15)};
  bool success =
      manager.executeProcedure("isPositive", args1, result, errorMsg);
  EXPECT_TRUE(success);
  EXPECT_TRUE(std::get<bool>(result) == true);

  // 2. Compute square sum
  std::vector<Value> args2 = {static_cast<int32_t>(5),
                              static_cast<int32_t>(12)};
  success =
      manager.executeProcedure("computeSquareSum", args2, result, errorMsg);
  EXPECT_TRUE(success);
  EXPECT_TRUE(std::get<int32_t>(result) == 169); // 5^2 + 12^2 = 25 + 144

  // 3. Test range validation
  std::vector<Value> args3 = {static_cast<int32_t>(50), static_cast<int32_t>(1),
                              static_cast<int32_t>(100)};
  success = manager.executeProcedure("isInRange", args3, result, errorMsg);
  EXPECT_TRUE(success);
  EXPECT_TRUE(std::get<bool>(result) == true);

  // 4. Test external clamp function
  std::string clampSource =
      "int32 safeValue(int32 x) { return clamp(x, 0, 100); }";
  manager.loadScriptSource(clampSource, "clamp_test.script", errors);

  std::vector<Value> args4 = {static_cast<int32_t>(150)};
  success = manager.executeProcedure("safeValue", args4, result, errorMsg);
  EXPECT_TRUE(success);
  EXPECT_TRUE(std::get<int32_t>(result) == 100); // Clamped to max

}

TEST(MultiFileTest, UnregisterExternalFunctionImpact) {

  ScriptManager manager;

  // Register external function
  manager.registerExternalFunction("power",
                                   [](const std::vector<Value> &args) -> Value {
                                     int32_t base = std::get<int32_t>(args[0]);
                                     int32_t exp = std::get<int32_t>(args[1]);
                                     int32_t result = 1;
                                     for (int32_t i = 0; i < exp; ++i) {
                                       result *= base;
                                     }
                                     return result;
                                   });

  std::string source = "int32 cube(int32 x) { return power(x, 3); }";

  std::vector<CompilationError> errors;
  manager.loadScriptSource(source, "power_test.script", errors);

  std::string errorMsg;
  Value result;
  std::vector<Value> args = {static_cast<int32_t>(4)};

  // Should work
  bool success = manager.executeProcedure("cube", args, result, errorMsg);
  EXPECT_TRUE(success);
  EXPECT_TRUE(std::get<int32_t>(result) == 64);

  // Unregister the external function
  manager.unregisterExternalFunction("power");

  // Should now fail
  success = manager.executeProcedure("cube", args, result, errorMsg);
  EXPECT_TRUE(!success);
  EXPECT_TRUE(errorMsg.find("Undefined function") != std::string::npos);

}

