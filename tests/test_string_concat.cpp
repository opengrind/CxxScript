#include "ScriptManager.h"
#include <gtest/gtest.h>

using namespace Script;

TEST(StringConcatTest, StringConcatenation) {
  std::cout << "Testing string concatenation with different types..."
            << std::endl;

  ScriptManager manager;
  std::vector<CompilationError> errors;

  // Test string + string
  std::string source1 = R"(
    string concat1() {
      string a = "Hello";
      string b = " World";
      return a + b;
    }
  )";
  manager.loadScriptSource(source1, "test1.script", errors);
  EXPECT_TRUE(errors.empty());

  std::string errorMsg;
  Value result;
  std::vector<Value> noArgs;
  bool success = manager.executeProcedure("concat1", noArgs, result, errorMsg);
  EXPECT_TRUE(success);
  EXPECT_TRUE(std::get<std::string>(result) == "Hello World");
  std::cout << "  ✓ string + string = \"Hello World\"" << std::endl;

  // Test string + int32
  std::string source2 = R"(
    string concat2() {
      string a = "Number: ";
      int32 b = 42;
      return a + b;
    }
  )";
  manager.loadScriptSource(source2, "test2.script", errors);
  EXPECT_TRUE(errors.empty());

  success = manager.executeProcedure("concat2", noArgs, result, errorMsg);
  EXPECT_TRUE(success);
  EXPECT_TRUE(std::get<std::string>(result) == "Number: 42");
  std::cout << "  ✓ string + int32 = \"Number: 42\"" << std::endl;

  // Test int32 + string
  std::string source3 = R"(
    string concat3() {
      int32 a = 100;
      string b = " items";
      return a + b;
    }
  )";
  manager.loadScriptSource(source3, "test3.script", errors);
  EXPECT_TRUE(errors.empty());

  success = manager.executeProcedure("concat3", noArgs, result, errorMsg);
  EXPECT_TRUE(success);
  EXPECT_TRUE(std::get<std::string>(result) == "100 items");
  std::cout << "  ✓ int32 + string = \"100 items\"" << std::endl;

  // Test string + bool
  std::string source4 = R"(
    string concat4() {
      string a = "Value is: ";
      bool b = true;
      return a + b;
    }
  )";
  manager.loadScriptSource(source4, "test4.script", errors);
  EXPECT_TRUE(errors.empty());

  success = manager.executeProcedure("concat4", noArgs, result, errorMsg);
  EXPECT_TRUE(success);
  EXPECT_TRUE(std::get<std::string>(result) == "Value is: true");
  std::cout << "  ✓ string + bool = \"Value is: true\"" << std::endl;

  // Test bool + string
  std::string source5 = R"(
    string concat5() {
      bool a = false;
      string b = " is the answer";
      return a + b;
    }
  )";
  manager.loadScriptSource(source5, "test5.script", errors);
  EXPECT_TRUE(errors.empty());

  success = manager.executeProcedure("concat5", noArgs, result, errorMsg);
  EXPECT_TRUE(success);
  EXPECT_TRUE(std::get<std::string>(result) == "false is the answer");
  std::cout << "  ✓ bool + string = \"false is the answer\"" << std::endl;

  // Test multiple concatenations
  std::string source6 = R"(
    string concat6() {
      string a = "Result: ";
      int32 b = 10;
      string c = " + ";
      int32 d = 20;
      string e = " = ";
      int32 f = 30;
      return a + b + c + d + e + f;
    }
  )";
  manager.loadScriptSource(source6, "test6.script", errors);
  EXPECT_TRUE(errors.empty());

  success = manager.executeProcedure("concat6", noArgs, result, errorMsg);
  EXPECT_TRUE(success);
  EXPECT_TRUE(std::get<std::string>(result) == "Result: 10 + 20 = 30");
  std::cout << "  ✓ Multiple concatenations = \"Result: 10 + 20 = 30\""
            << std::endl;

  // Test with different integer types
  std::string source7 = R"(
    string concat7() {
      string prefix = "Values: ";
      int8 a = 127;
      string sep1 = ", ";
      uint16 b = 255;
      string sep2 = ", ";
      int32 c = 2147483647;
      return prefix + a + sep1 + b + sep2 + c;
    }
  )";
  manager.loadScriptSource(source7, "test7.script", errors);
  EXPECT_TRUE(errors.empty());

  success = manager.executeProcedure("concat7", noArgs, result, errorMsg);
  EXPECT_TRUE(success);
  EXPECT_TRUE(std::get<std::string>(result) == "Values: 127, 255, 2147483647");
  std::cout << "  ✓ Different integer types = \"Values: 127, 255, "
               "2147483647\""
            << std::endl;

  // Test concatenation in expressions
  std::string source8 = R"(
    string concat8(int32 x, int32 y) {
      return "Sum of " + x + " and " + y + " is " + (x + y);
    }
  )";
  manager.loadScriptSource(source8, "test8.script", errors);
  EXPECT_TRUE(errors.empty());

  std::vector<Value> args8 = {static_cast<int32_t>(5), static_cast<int32_t>(7)};
  success = manager.executeProcedure("concat8", args8, result, errorMsg);
  EXPECT_TRUE(success);
  EXPECT_TRUE(std::get<std::string>(result) == "Sum of 5 and 7 is 12");
  std::cout << "  ✓ Expression concatenation = \"Sum of 5 and 7 is 12\""
            << std::endl;

  // Test with uint types
  std::string source9 = R"(
    string concat9() {
      string msg = "Unsigned: ";
      uint32 val = 4294967295;
      return msg + val;
    }
  )";
  manager.loadScriptSource(source9, "test9.script", errors);
  EXPECT_TRUE(errors.empty());

  success = manager.executeProcedure("concat9", noArgs, result, errorMsg);
  EXPECT_TRUE(success);
  EXPECT_TRUE(std::get<std::string>(result) == "Unsigned: 4294967295");
  std::cout << "  ✓ string + uint32 = \"Unsigned: 4294967295\"" << std::endl;

  // Test empty string concatenation
  std::string source10 = R"(
    string concat10() {
      string empty = "";
      int32 num = 123;
      return empty + num + empty;
    }
  )";
  manager.loadScriptSource(source10, "test10.script", errors);
  EXPECT_TRUE(errors.empty());

  success = manager.executeProcedure("concat10", noArgs, result, errorMsg);
  EXPECT_TRUE(success);
  EXPECT_TRUE(std::get<std::string>(result) == "123");
  std::cout << "  ✓ Empty string concatenation = \"123\"" << std::endl;
}

TEST(StringConcatTest, ComplexConcatenation) {

  ScriptManager manager;
  std::vector<CompilationError> errors;

  // Test in conditional context
  std::string source1 = R"(
    string format(int32 x) {
      if (x > 0) {
        return "Positive: " + x;
      } else {
        return "Non-positive: " + x;
      }
    }
  )";
  manager.loadScriptSource(source1, "complex1.script", errors);
  EXPECT_TRUE(errors.empty());

  std::string errorMsg;
  Value result;
  std::vector<Value> args1 = {static_cast<int32_t>(10)};
  bool success = manager.executeProcedure("format", args1, result, errorMsg);
  EXPECT_TRUE(success);
  EXPECT_TRUE(std::get<std::string>(result) == "Positive: 10");

  args1[0] = static_cast<int32_t>(-5);
  success = manager.executeProcedure("format", args1, result, errorMsg);
  EXPECT_TRUE(success);
  EXPECT_TRUE(std::get<std::string>(result) == "Non-positive: -5");
  std::cout << "  ✓ Concatenation in conditionals" << std::endl;

  // Test in loops
  std::string source2 = R"(
    string buildString(int32 n) {
      string result = "Numbers: ";
      int32 i = 0;
      while (i < n) {
        result = result + i;
        if (i < n - 1) {
          result = result + ", ";
        }
        i = i + 1;
      }
      return result;
    }
  )";
  manager.loadScriptSource(source2, "complex2.script", errors);
  EXPECT_TRUE(errors.empty());

  std::vector<Value> args2 = {static_cast<int32_t>(5)};
  success = manager.executeProcedure("buildString", args2, result, errorMsg);
  EXPECT_TRUE(success);
  EXPECT_TRUE(std::get<std::string>(result) == "Numbers: 0, 1, 2, 3, 4");
  std::cout << "  ✓ Concatenation in loops = \"Numbers: 0, 1, 2, 3, 4\""
            << std::endl;

  // Test with procedure calls
  std::string source3 = R"(
    int32 compute(int32 a, int32 b) {
      return a * b;
    }
    
    string formatResult(int32 x, int32 y) {
      return x + " * " + y + " = " + compute(x, y);
    }
  )";
  manager.loadScriptSource(source3, "complex3.script", errors);
  EXPECT_TRUE(errors.empty());

  std::vector<Value> args3 = {static_cast<int32_t>(6), static_cast<int32_t>(7)};
  success = manager.executeProcedure("formatResult", args3, result, errorMsg);
  EXPECT_TRUE(success);
  EXPECT_TRUE(std::get<std::string>(result) == "6 * 7 = 42");
  std::cout << "  ✓ Concatenation with procedure calls = \"6 * 7 = 42\""
            << std::endl;
}

