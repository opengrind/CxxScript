#include "ScriptManager.h"
#include <gtest/gtest.h>

using namespace Script;

TEST(InterpreterTest, BasicExecution) {
  std::string source = R"(
        int32 add(int32 a, int32 b) {
            return a + b;
        }
    )";

  ScriptManager manager;
  std::vector<CompilationError> errors;

  bool success = manager.loadScriptSource(source, "test.script", errors);
  ASSERT_TRUE(success);
  ASSERT_TRUE(errors.empty());

  std::vector<Value> args = {static_cast<int32_t>(10),
                             static_cast<int32_t>(20)};
  Value result;
  std::string errorMsg;

  success = manager.executeProcedure("add", args, result, errorMsg);
  ASSERT_TRUE(success);
  EXPECT_EQ(std::get<int32_t>(result), 30);
}

TEST(InterpreterTest, ControlFlow) {
  std::string source = R"(
        bool isPositive(int32 x) {
            if (x > 0) {
                return true;
            } else {
                return false;
            }
        }
    )";

  ScriptManager manager;
  std::vector<CompilationError> errors;

  bool success = manager.loadScriptSource(source, "test.script", errors);
  ASSERT_TRUE(success);

  std::vector<Value> args1 = {static_cast<int32_t>(10)};
  Value result1;
  std::string errorMsg;

  success = manager.executeProcedure("isPositive", args1, result1, errorMsg);
  ASSERT_TRUE(success);
  EXPECT_TRUE(std::get<bool>(result1));

  std::vector<Value> args2 = {static_cast<int32_t>(-5)};
  Value result2;

  success = manager.executeProcedure("isPositive", args2, result2, errorMsg);
  ASSERT_TRUE(success);
  EXPECT_FALSE(std::get<bool>(result2));
}

TEST(InterpreterTest, LoopExecution) {
  std::string source = R"(
        int32 factorial(int32 n) {
            int32 result = 1;
            for (int32 i = 1; i <= n; i += 1) {
                result *= i;
            }
            return result;
        }
    )";

  ScriptManager manager;
  std::vector<CompilationError> errors;

  bool success = manager.loadScriptSource(source, "test.script", errors);
  ASSERT_TRUE(success);

  std::vector<Value> args = {static_cast<int32_t>(5)};
  Value result;
  std::string errorMsg;

  success = manager.executeProcedure("factorial", args, result, errorMsg);
  ASSERT_TRUE(success);
  EXPECT_EQ(std::get<int32_t>(result), 120); // 5! = 120
}

TEST(InterpreterTest, ProcedureCall) {
  std::string source = R"(
        int32 square(int32 x) {
            return x * x;
        }
        
        int32 sumOfSquares(int32 a, int32 b) {
            return square(a) + square(b);
        }
    )";

  ScriptManager manager;
  std::vector<CompilationError> errors;

  bool success = manager.loadScriptSource(source, "test.script", errors);
  ASSERT_TRUE(success);

  std::vector<Value> args = {static_cast<int32_t>(3), static_cast<int32_t>(4)};
  Value result;
  std::string errorMsg;

  success = manager.executeProcedure("sumOfSquares", args, result, errorMsg);
  ASSERT_TRUE(success);
  EXPECT_EQ(std::get<int32_t>(result), 25); // 3^2 + 4^2 = 9 + 16 = 25
}

TEST(InterpreterTest, ComplexExample) {
  std::string source = R"(
        bool calculate(int32 arg1, int32 arg2) {
            int32 var1 = arg1 + 56;
            int32 var2 = arg2 / 34;
            int32 total = var1 + var2;
            
            if (total > 43) {
                return true;
            }
            
            return false;
        }
    )";

  ScriptManager manager;
  std::vector<CompilationError> errors;

  bool success = manager.loadScriptSource(source, "test.script", errors);
  ASSERT_TRUE(success);

  std::vector<Value> args1 = {static_cast<int32_t>(10),
                              static_cast<int32_t>(100)};
  Value result1;
  std::string errorMsg;

  success = manager.executeProcedure("calculate", args1, result1, errorMsg);
  ASSERT_TRUE(success);
  // var1 = 10 + 56 = 66, var2 = 100 / 34 = 2, total = 68 > 43 -> true
  EXPECT_TRUE(std::get<bool>(result1));

  std::vector<Value> args2 = {static_cast<int32_t>(0), static_cast<int32_t>(0)};
  Value result2;

  success = manager.executeProcedure("calculate", args2, result2, errorMsg);
  ASSERT_TRUE(success);
  // var1 = 0 + 56 = 56, var2 = 0 / 34 = 0, total = 56 > 43 -> true
  EXPECT_TRUE(std::get<bool>(result2));
}

TEST(InterpreterTest, ExternalFunction) {
  std::string source = R"(
        int32 test() {
            int32 result = externalFunc(42);
            return result;
        }
    )";

  ScriptManager manager;

  // Register external function
  manager.registerExternalFunction("externalFunc",
                                   [](const std::vector<Value> &args) -> Value {
                                     int32_t input = std::get<int32_t>(args[0]);
                                     return input * 2; // Double the input
                                   });

  std::vector<CompilationError> errors;
  bool success = manager.loadScriptSource(source, "test.script", errors);
  ASSERT_TRUE(success);

  std::vector<Value> args;
  Value result;
  std::string errorMsg;

  success = manager.executeProcedure("test", args, result, errorMsg);
  ASSERT_TRUE(success);
  EXPECT_EQ(std::get<int32_t>(result), 84); // 42 * 2
}

TEST(InterpreterTest, MultipleProceduresInSource) {
  std::string source = "int32 add(int32 a, int32 b) { return a + b; }"
                       "int32 sub(int32 a, int32 b) { return a - b; }"
                       "int32 mul(int32 a, int32 b) { return a * b; }"
                       "int32 div(int32 a, int32 b) { return a / b; }";

  ScriptManager manager;
  std::vector<CompilationError> errors;
  bool success = manager.loadScriptSource(source, "math.script", errors);
  ASSERT_TRUE(success);

  EXPECT_TRUE(manager.hasProcedure("add"));
  EXPECT_TRUE(manager.hasProcedure("sub"));
  EXPECT_TRUE(manager.hasProcedure("mul"));
  EXPECT_TRUE(manager.hasProcedure("div"));

  std::vector<Value> args = {static_cast<int32_t>(20), static_cast<int32_t>(5)};
  Value result;
  std::string errorMsg;

  success = manager.executeProcedure("add", args, result, errorMsg);
  ASSERT_TRUE(success);
  EXPECT_EQ(std::get<int32_t>(result), 25);

  success = manager.executeProcedure("sub", args, result, errorMsg);
  ASSERT_TRUE(success);
  EXPECT_EQ(std::get<int32_t>(result), 15);

  success = manager.executeProcedure("mul", args, result, errorMsg);
  ASSERT_TRUE(success);
  EXPECT_EQ(std::get<int32_t>(result), 100);

  success = manager.executeProcedure("div", args, result, errorMsg);
  ASSERT_TRUE(success);
  EXPECT_EQ(std::get<int32_t>(result), 4);
}

TEST(InterpreterTest, NestedProcedureCalls) {
  std::string source =
      "int32 triple(int32 x) { return x * 3; }"
      "int32 doubleTriple(int32 x) { int32 t = triple(x); return t * 2; }"
      "int32 addTriples(int32 a, int32 b) { return triple(a) + triple(b); }";

  ScriptManager manager;
  std::vector<CompilationError> errors;
  bool success = manager.loadScriptSource(source, "nested.script", errors);
  ASSERT_TRUE(success);

  std::vector<Value> args1 = {static_cast<int32_t>(5)};
  Value result1;
  std::string errorMsg;

  success = manager.executeProcedure("doubleTriple", args1, result1, errorMsg);
  ASSERT_TRUE(success);
  EXPECT_EQ(std::get<int32_t>(result1), 30);

  std::vector<Value> args2 = {static_cast<int32_t>(3), static_cast<int32_t>(4)};
  Value result2;

  success = manager.executeProcedure("addTriples", args2, result2, errorMsg);
  ASSERT_TRUE(success);
  EXPECT_EQ(std::get<int32_t>(result2), 21);
}

TEST(InterpreterTest, RecursiveProcedures) {
  std::string source = "int32 factorial(int32 n) { if (n <= 1) { return 1; } "
                       "return n * factorial(n - 1); }"
                       "int32 fibonacci(int32 n) { if (n <= 1) { return n; } "
                       "return fibonacci(n - 1) + fibonacci(n - 2); }";

  ScriptManager manager;
  std::vector<CompilationError> errors;
  bool success = manager.loadScriptSource(source, "recursive.script", errors);
  ASSERT_TRUE(success);

  std::vector<Value> args1 = {static_cast<int32_t>(6)};
  Value result1;
  std::string errorMsg;

  success = manager.executeProcedure("factorial", args1, result1, errorMsg);
  ASSERT_TRUE(success);
  EXPECT_EQ(std::get<int32_t>(result1), 720);

  std::vector<Value> args2 = {static_cast<int32_t>(7)};
  Value result2;

  success = manager.executeProcedure("fibonacci", args2, result2, errorMsg);
  ASSERT_TRUE(success);
  EXPECT_EQ(std::get<int32_t>(result2), 13);
}

TEST(InterpreterTest, ExternalFunctionCallbacks) {

  std::string source = "int32 useExternalFunctions(int32 a, int32 b) {"
                       "  int32 sum = Add(a, b);"
                       "  int32 product = Multiply(a, b);"
                       "  int32 result = Max(sum, product);"
                       "  return result;"
                       "}"
                       "string formatMessage(int32 value) {"
                       "  string msg = FormatInt(value);"
                       "  return msg;"
                       "}";

  ScriptManager manager;

  // Register multiple external functions
  manager.registerExternalFunction(
      "Add", [](const std::vector<Value> &arguments) -> Value {
        std::cout << "    External function called: Add with "
                  << arguments.size() << " arguments" << std::endl;
        int32_t a = std::get<int32_t>(arguments[0]);
        int32_t b = std::get<int32_t>(arguments[1]);
        return a + b;
      });

  manager.registerExternalFunction(
      "Multiply", [](const std::vector<Value> &arguments) -> Value {
        std::cout << "    External function called: Multiply with "
                  << arguments.size() << " arguments" << std::endl;
        int32_t a = std::get<int32_t>(arguments[0]);
        int32_t b = std::get<int32_t>(arguments[1]);
        return a * b;
      });

  manager.registerExternalFunction(
      "Max", [](const std::vector<Value> &arguments) -> Value {
        std::cout << "    External function called: Max with "
                  << arguments.size() << " arguments" << std::endl;
        int32_t a = std::get<int32_t>(arguments[0]);
        int32_t b = std::get<int32_t>(arguments[1]);
        return (a > b) ? a : b;
      });

  manager.registerExternalFunction(
      "FormatInt", [](const std::vector<Value> &arguments) -> Value {
        std::cout << "    External function called: FormatInt with "
                  << arguments.size() << " arguments" << std::endl;
        int32_t val = std::get<int32_t>(arguments[0]);
        return std::string("Value: ") + std::to_string(val);
      });

  std::vector<CompilationError> errors;
  bool success = manager.loadScriptSource(source, "external.script", errors);
  EXPECT_TRUE(success);

  // Test calling procedure that uses multiple external functions
  std::vector<Value> args1 = {static_cast<int32_t>(5),
                              static_cast<int32_t>(10)};
  Value result1;
  std::string errorMsg;

  success = manager.executeProcedure("useExternalFunctions", args1, result1,
                                     errorMsg);
  EXPECT_TRUE(success);
  // sum = 15, product = 50, max = 50
  EXPECT_TRUE(std::get<int32_t>(result1) == 50);

  // Test external function returning string
  std::vector<Value> args2 = {static_cast<int32_t>(42)};
  Value result2;

  success = manager.executeProcedure("formatMessage", args2, result2, errorMsg);
  EXPECT_TRUE(success);
  EXPECT_TRUE(std::get<std::string>(result2) == "Value: 42");

}

TEST(InterpreterTest, StringReturnProcedures) {

  std::string source = "string greet(string name) {"
                       "  string greeting = \"Hello, \";"
                       "  greeting += name;"
                       "  greeting += \"!\";"
                       "  return greeting;"
                       "}"
                       "string concat(string a, string b, string c) {"
                       "  string result = a;"
                       "  result += b;"
                       "  result += c;"
                       "  return result;"
                       "}"
                       "string repeat(string text, int32 count) {"
                       "  string result = \"\";"
                       "  for (int32 i = 0; i < count; i += 1) {"
                       "    result += text;"
                       "  }"
                       "  return result;"
                       "}";

  ScriptManager manager;
  std::vector<CompilationError> errors;
  bool success = manager.loadScriptSource(source, "string_test.script", errors);
  EXPECT_TRUE(success);

  std::string errorMsg;
  Value result;

  // Test simple string return
  std::vector<Value> args1 = {std::string("World")};
  success = manager.executeProcedure("greet", args1, result, errorMsg);
  EXPECT_TRUE(success);
  EXPECT_TRUE(std::get<std::string>(result) == "Hello, World!");

  // Test string concatenation with multiple parameters
  std::vector<Value> args2 = {std::string("One"), std::string("Two"),
                              std::string("Three")};
  success = manager.executeProcedure("concat", args2, result, errorMsg);
  EXPECT_TRUE(success);
  EXPECT_TRUE(std::get<std::string>(result) == "OneTwoThree");

  // Test string building in loop
  std::vector<Value> args3 = {std::string("AB"), static_cast<int32_t>(3)};
  success = manager.executeProcedure("repeat", args3, result, errorMsg);
  EXPECT_TRUE(success);
  EXPECT_TRUE(std::get<std::string>(result) == "ABABAB");

}

TEST(InterpreterTest, StringProcedureChaining) {

  std::string source = "string toUpper(string text) {"
                       "  return text;"
                       "}"
                       "string addPrefix(string text) {"
                       "  return \"PREFIX_\" + text;"
                       "}"
                       "string addSuffix(string text) {"
                       "  return text + \"_SUFFIX\";"
                       "}"
                       "string formatText(string input) {"
                       "  string prefixed = addPrefix(input);"
                       "  string formatted = addSuffix(prefixed);"
                       "  return formatted;"
                       "}";

  ScriptManager manager;
  std::vector<CompilationError> errors;
  bool success = manager.loadScriptSource(source, "chain_test.script", errors);
  EXPECT_TRUE(success);

  std::string errorMsg;
  Value result;

  // Test chained string procedures
  std::vector<Value> args = {std::string("test")};
  success = manager.executeProcedure("formatText", args, result, errorMsg);
  EXPECT_TRUE(success);
  EXPECT_TRUE(std::get<std::string>(result) == "PREFIX_test_SUFFIX");

}

TEST(InterpreterTest, StringConditionals) {

  std::string source = "string getMessage(bool success) {"
                       "  if (success) {"
                       "    return \"Operation successful\";"
                       "  } else {"
                       "    return \"Operation failed\";"
                       "  }"
                       "}"
                       "string selectGreeting(int32 hour) {"
                       "  if (hour < 12) {"
                       "    return \"Good morning\";"
                       "  }"
                       "  if (hour < 18) {"
                       "    return \"Good afternoon\";"
                       "  }"
                       "  return \"Good evening\";"
                       "}";

  ScriptManager manager;
  std::vector<CompilationError> errors;
  bool success =
      manager.loadScriptSource(source, "conditional_test.script", errors);
  EXPECT_TRUE(success);

  std::string errorMsg;
  Value result;

  // Test conditional string return - true case
  std::vector<Value> args1 = {true};
  success = manager.executeProcedure("getMessage", args1, result, errorMsg);
  EXPECT_TRUE(success);
  EXPECT_TRUE(std::get<std::string>(result) == "Operation successful");

  // Test conditional string return - false case
  std::vector<Value> args2 = {false};
  success = manager.executeProcedure("getMessage", args2, result, errorMsg);
  EXPECT_TRUE(success);
  EXPECT_TRUE(std::get<std::string>(result) == "Operation failed");

  // Test multiple conditionals - morning
  std::vector<Value> args3 = {static_cast<int32_t>(9)};
  success = manager.executeProcedure("selectGreeting", args3, result, errorMsg);
  EXPECT_TRUE(success);
  EXPECT_TRUE(std::get<std::string>(result) == "Good morning");

  // Test multiple conditionals - afternoon
  std::vector<Value> args4 = {static_cast<int32_t>(14)};
  success = manager.executeProcedure("selectGreeting", args4, result, errorMsg);
  EXPECT_TRUE(success);
  EXPECT_TRUE(std::get<std::string>(result) == "Good afternoon");

  // Test multiple conditionals - evening
  std::vector<Value> args5 = {static_cast<int32_t>(20)};
  success = manager.executeProcedure("selectGreeting", args5, result, errorMsg);
  EXPECT_TRUE(success);
  EXPECT_TRUE(std::get<std::string>(result) == "Good evening");

}

TEST(InterpreterTest, MixedTypeStringProcedures) {

  std::string source = "string numberToWord(int32 n) {"
                       "  if (n == 0) { return \"zero\"; }"
                       "  if (n == 1) { return \"one\"; }"
                       "  if (n == 2) { return \"two\"; }"
                       "  if (n == 3) { return \"three\"; }"
                       "  return \"many\";"
                       "}"
                       "string boolToString(bool value) {"
                       "  if (value) {"
                       "    return \"yes\";"
                       "  }"
                       "  return \"no\";"
                       "}"
                       "string buildReport(int32 count, bool status) {"
                       "  string countStr = numberToWord(count);"
                       "  string statusStr = boolToString(status);"
                       "  string report = \"Count: \";"
                       "  report += countStr;"
                       "  report += \", Status: \";"
                       "  report += statusStr;"
                       "  return report;"
                       "}";

  ScriptManager manager;
  std::vector<CompilationError> errors;
  bool success = manager.loadScriptSource(source, "mixed_test.script", errors);
  EXPECT_TRUE(success);

  std::string errorMsg;
  Value result;

  // Test number to word conversion
  std::vector<Value> args1 = {static_cast<int32_t>(2)};
  success = manager.executeProcedure("numberToWord", args1, result, errorMsg);
  EXPECT_TRUE(success);
  EXPECT_TRUE(std::get<std::string>(result) == "two");

  // Test bool to string conversion
  std::vector<Value> args2 = {true};
  success = manager.executeProcedure("boolToString", args2, result, errorMsg);
  EXPECT_TRUE(success);
  EXPECT_TRUE(std::get<std::string>(result) == "yes");

  // Test mixed type report building
  std::vector<Value> args3 = {static_cast<int32_t>(3), false};
  success = manager.executeProcedure("buildReport", args3, result, errorMsg);
  EXPECT_TRUE(success);
  EXPECT_TRUE(std::get<std::string>(result) == "Count: three, Status: no");

}
