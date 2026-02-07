#include "ScriptManager.h"
#include "DataTypes.h"
#include <gtest/gtest.h>

using namespace Script;

TEST(ExternalFunctionsTest, MultipleExternalFunctions) {
  std::cout << "Testing multiple external function registration..."
            << std::endl;

  std::string source = "int32 calculate(int32 x) {"
                       "  int32 doubled = twice(x);"
                       "  int32 tripled = triple(x);"
                       "  int32 result = add(doubled, tripled);"
                       "  return result;"
                       "}";

  ScriptManager manager;

  // Register multiple external functions
  manager.registerExternalFunction("twice",
                                   [](const std::vector<Value> &args) -> Value {
                                     return std::get<int32_t>(args[0]) * 2;
                                   });

  manager.registerExternalFunction("triple",
                                   [](const std::vector<Value> &args) -> Value {
                                     return std::get<int32_t>(args[0]) * 3;
                                   });

  manager.registerExternalFunction(
      "add", [](const std::vector<Value> &args) -> Value {
        return std::get<int32_t>(args[0]) + std::get<int32_t>(args[1]);
      });

  std::vector<CompilationError> errors;
  bool success = manager.loadScriptSource(source, "multi_ext.script", errors);
  EXPECT_TRUE(success);

  // Test that all functions work together
  std::vector<Value> args = {static_cast<int32_t>(5)};
  Value result;
  std::string errorMsg;

  success = manager.executeProcedure("calculate", args, result, errorMsg);
  EXPECT_TRUE(success);
  // double(5) = 10, triple(5) = 15, add(10, 15) = 25
  EXPECT_TRUE(std::get<int32_t>(result) == 25);

}

TEST(ExternalFunctionsTest, ExternalFunctionOverwrite) {

  std::string source = "int32 test(int32 x) {"
                       "  return getValue(x);"
                       "}";

  ScriptManager manager;

  // Register initial function
  manager.registerExternalFunction("getValue",
                                   [](const std::vector<Value> &args) -> Value {
                                     return std::get<int32_t>(args[0]) * 10;
                                   });

  std::vector<CompilationError> errors;
  bool success = manager.loadScriptSource(source, "overwrite.script", errors);
  EXPECT_TRUE(success);

  std::vector<Value> args = {static_cast<int32_t>(5)};
  Value result;
  std::string errorMsg;

  success = manager.executeProcedure("test", args, result, errorMsg);
  EXPECT_TRUE(success);
  EXPECT_TRUE(std::get<int32_t>(result) == 50); // 5 * 10

  // Overwrite the function
  manager.registerExternalFunction("getValue",
                                   [](const std::vector<Value> &args) -> Value {
                                     return std::get<int32_t>(args[0]) * 20;
                                   });

  success = manager.executeProcedure("test", args, result, errorMsg);
  EXPECT_TRUE(success);
  EXPECT_TRUE(std::get<int32_t>(result) == 100); // 5 * 20

}

TEST(ExternalFunctionsTest, UnregisterExternalFunction) {

  std::string source = "int32 test(int32 x) {"
                       "  return compute(x);"
                       "}";

  ScriptManager manager;

  // Register function
  manager.registerExternalFunction("compute",
                                   [](const std::vector<Value> &args) -> Value {
                                     return std::get<int32_t>(args[0]) * 2;
                                   });

  EXPECT_TRUE(manager.hasExternalFunction("compute"));

  std::vector<CompilationError> errors;
  bool success = manager.loadScriptSource(source, "unreg.script", errors);
  EXPECT_TRUE(success);

  std::vector<Value> args = {static_cast<int32_t>(10)};
  Value result;
  std::string errorMsg;

  // Should work
  success = manager.executeProcedure("test", args, result, errorMsg);
  EXPECT_TRUE(success);
  EXPECT_TRUE(std::get<int32_t>(result) == 20);

  // Unregister the function
  manager.unregisterExternalFunction("compute");
  EXPECT_TRUE(!manager.hasExternalFunction("compute"));

  // Should fail now
  success = manager.executeProcedure("test", args, result, errorMsg);
  EXPECT_TRUE(!success);
  EXPECT_TRUE(errorMsg.find("Undefined function") != std::string::npos);

}

TEST(ExternalFunctionsTest, MixedInternalAndExternal) {

  std::string source = "int32 internal(int32 x) {"
                       "  return x * 2;"
                       "}"
                       "int32 mixed(int32 x) {"
                       "  int32 a = internal(x);"
                       "  int32 b = external(x);"
                       "  return a + b;"
                       "}";

  ScriptManager manager;

  manager.registerExternalFunction("external",
                                   [](const std::vector<Value> &args) -> Value {
                                     return std::get<int32_t>(args[0]) * 3;
                                   });

  std::vector<CompilationError> errors;
  bool success = manager.loadScriptSource(source, "mixed.script", errors);
  EXPECT_TRUE(success);

  std::vector<Value> args = {static_cast<int32_t>(5)};
  Value result;
  std::string errorMsg;

  success = manager.executeProcedure("mixed", args, result, errorMsg);
  EXPECT_TRUE(success);
  // internal(5) = 10, external(5) = 15, result = 25
  EXPECT_TRUE(std::get<int32_t>(result) == 25);

}

TEST(ExternalFunctionsTest, ExternalFunctionReturningDifferentTypes) {
  std::cout << "Testing external functions with different return types..."
            << std::endl;

  std::string source = "int32 getInt() { return getNumber(); }"
                       "string getString() { return getMessage(); }"
                       "bool getBool() { return isValid(); }";

  ScriptManager manager;

  manager.registerExternalFunction(
      "getNumber", [](const std::vector<Value> & /*args*/) -> Value {
        return static_cast<int32_t>(42);
      });

  manager.registerExternalFunction(
      "getMessage", [](const std::vector<Value> & /*args*/) -> Value {
        return std::string("Hello from C++");
      });

  manager.registerExternalFunction(
      "isValid",
      [](const std::vector<Value> & /*args*/) -> Value { return true; });

  std::vector<CompilationError> errors;
  bool success = manager.loadScriptSource(source, "types.script", errors);
  EXPECT_TRUE(success);

  Value result;
  std::string errorMsg;

  // Test int return
  std::vector<Value> noArgs;
  success = manager.executeProcedure("getInt", noArgs, result, errorMsg);
  EXPECT_TRUE(success);
  EXPECT_TRUE(std::get<int32_t>(result) == 42);

  // Test string return
  success = manager.executeProcedure("getString", noArgs, result, errorMsg);
  EXPECT_TRUE(success);
  EXPECT_TRUE(std::get<std::string>(result) == "Hello from C++");

  // Test bool return
  success = manager.executeProcedure("getBool", noArgs, result, errorMsg);
  EXPECT_TRUE(success);
  EXPECT_TRUE(std::get<bool>(result) == true);

  std::cout << "  ✓ External functions with different return types passed"
            << std::endl;
}

TEST(ExternalFunctionsTest, DoubleArgumentsAndReturn) {
  std::string source = "double area(double r) { return circle(r); }";

  ScriptManager manager;
  manager.registerExternalFunction(
      "circle", [](const std::vector<Value> &args) -> Value {
        double r = std::get<double>(args[0]);
        return 3.5 * r * r;
      });

  std::vector<CompilationError> errors;
  bool success = manager.loadScriptSource(source, "double_ext.script", errors);
  EXPECT_TRUE(success);

  Value result;
  std::string errorMsg;
  success = manager.executeProcedure("area", {2.0}, result, errorMsg);
  EXPECT_TRUE(success);
  EXPECT_DOUBLE_EQ(std::get<double>(result), 14.0);
}

TEST(ExternalFunctionsTest, ArrayReturnAndArgument) {
  std::string source =
      "int32 useReturn() { int32[] arr = makeArray(); return arr[0] + arr[1] + len(arr); }"
      "int32 useArg() { int32[] local = [3,4,5]; return head(local); }";

  ScriptManager manager;
  manager.registerExternalFunction(
      "makeArray", [](const std::vector<Value> & /*args*/) -> Value {
        return ValueHelper::createArray(
            TypeInfo(DataType::INT32),
            {static_cast<int32_t>(5), static_cast<int32_t>(6)});
      });

  manager.registerExternalFunction(
      "head", [](const std::vector<Value> &args) -> Value {
        const auto &elems = ValueHelper::arrayElements(args[0]);
        return elems.empty() ? static_cast<int32_t>(0)
                             : std::get<int32_t>(elems.front());
      });

  std::vector<CompilationError> errors;
  bool success = manager.loadScriptSource(source, "array_ext.script", errors);
  EXPECT_TRUE(success);

  Value result;
  std::string errorMsg;

  success = manager.executeProcedure("useReturn", {}, result, errorMsg);
  EXPECT_TRUE(success);
  // 5 + 6 + len=2 => 13
  EXPECT_EQ(std::get<int32_t>(result), 13);

  success = manager.executeProcedure("useArg", {}, result, errorMsg);
  EXPECT_TRUE(success);
  EXPECT_EQ(std::get<int32_t>(result), 3);
}

TEST(ExternalFunctionsTest, ClearResetsProceduresAndExternalFunctions) {

  std::string source = "int32 call() { return persist(); }";

  ScriptManager manager;

  manager.registerExternalFunction(
      "persist", [](const std::vector<Value> & /*args*/) -> Value {
        return static_cast<int32_t>(5);
      });

  std::vector<CompilationError> errors;
  bool success = manager.loadScriptSource(source, "persist.script", errors);
  EXPECT_TRUE(success);

  std::vector<Value> args;
  Value result;
  std::string errorMsg;

  success = manager.executeProcedure("call", args, result, errorMsg);
  EXPECT_TRUE(success);
  EXPECT_EQ(std::get<int32_t>(result), 5);
  EXPECT_TRUE(manager.hasExternalFunction("persist"));
  EXPECT_TRUE(manager.hasProcedure("call"));

  manager.clear();

  EXPECT_FALSE(manager.hasExternalFunction("persist"));
  EXPECT_FALSE(manager.hasProcedure("call"));

  // Re-register after clear to confirm clean state
  manager.registerExternalFunction(
      "persist", [](const std::vector<Value> & /*args*/) -> Value {
        return static_cast<int32_t>(7);
      });

  errors.clear();
  success = manager.loadScriptSource(source, "persist.script", errors);
  EXPECT_TRUE(success);

  success = manager.executeProcedure("call", args, result, errorMsg);
  EXPECT_TRUE(success);
  EXPECT_EQ(std::get<int32_t>(result), 7);

}

