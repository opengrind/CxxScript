#include "ScriptManager.h"
#include <gtest/gtest.h>

using namespace Script;

TEST(EscapeSequencesTest, EscapeSequences) {

  ScriptManager manager;
  std::vector<CompilationError> errors;

  // Test all escape sequences
  std::string source = R"(
    string testQuote() {
      string s = "He said \"Hello\"";
      return s;
    }
    
    string testBackslash() {
      string s = "Path: C:\\Users\\Name";
      return s;
    }
    
    string testNewline() {
      string s = "Line1\nLine2";
      return s;
    }
    
    string testTab() {
      string s = "Col1\tCol2\tCol3";
      return s;
    }
    
    string testMixed() {
      string s = "Quote: \"test\"\nPath: C:\\dir\nTab:\there";
      return s;
    }
    
    string testInMessage() {
      string name = "John";
      string message = "User \"" + name + "\" logged in";
      return message;
    }
  )";

  if (!manager.loadScriptSource(source, "test_escape.script", errors)) {
    std::cout << "Compilation errors:" << std::endl;
    for (const auto &err : errors) {
      std::cout << "  " << err.toString() << std::endl;
    }
    EXPECT_TRUE(false);
  }

  Value result;
  std::string errorMsg;
  std::vector<Value> args;

  // Test escaped quotes
  {
    bool success =
        manager.executeProcedure("testQuote", args, result, errorMsg);
    EXPECT_TRUE(success);
    std::string str = std::get<std::string>(result);
    std::cout << "  ✓ Escaped quotes: [" << str << "]" << std::endl;
    EXPECT_TRUE(str == "He said \"Hello\"");
  }

  // Test escaped backslash
  {
    bool success =
        manager.executeProcedure("testBackslash", args, result, errorMsg);
    EXPECT_TRUE(success);
    std::string str = std::get<std::string>(result);
    std::cout << "  ✓ Escaped backslash: [" << str << "]" << std::endl;
    EXPECT_TRUE(str == "Path: C:\\Users\\Name");
  }

  // Test newline
  {
    bool success =
        manager.executeProcedure("testNewline", args, result, errorMsg);
    EXPECT_TRUE(success);
    std::string str = std::get<std::string>(result);
    std::cout << "  ✓ Newline: [";
    for (char c : str) {
      if (c == '\n')
        std::cout << "\\n";
      else
        std::cout << c;
    }
    std::cout << "]" << std::endl;
    EXPECT_TRUE(str == "Line1\nLine2");
  }

  // Test tab
  {
    bool success = manager.executeProcedure("testTab", args, result, errorMsg);
    EXPECT_TRUE(success);
    std::string str = std::get<std::string>(result);
    std::cout << "  ✓ Tab: [";
    for (char c : str) {
      if (c == '\t')
        std::cout << "\\t";
      else
        std::cout << c;
    }
    std::cout << "]" << std::endl;
    EXPECT_TRUE(str == "Col1\tCol2\tCol3");
  }

  // Test mixed
  {
    bool success =
        manager.executeProcedure("testMixed", args, result, errorMsg);
    EXPECT_TRUE(success);
    std::string str = std::get<std::string>(result);
    std::cout << "  ✓ Mixed escapes: ";
    for (char c : str) {
      if (c == '\n')
        std::cout << "\\n";
      else if (c == '\t')
        std::cout << "\\t";
      else
        std::cout << c;
    }
    std::cout << std::endl;
  }

  // Test in concatenation
  {
    bool success =
        manager.executeProcedure("testInMessage", args, result, errorMsg);
    EXPECT_TRUE(success);
    std::string str = std::get<std::string>(result);
    std::cout << "  ✓ In concatenation: [" << str << "]" << std::endl;
    EXPECT_TRUE(str == "User \"John\" logged in");
  }
}

TEST(EscapeSequencesTest, UnsupportedEscapes) {

  ScriptManager manager;
  std::vector<CompilationError> errors;

  // Unsupported escapes should keep the backslash
  std::string source = R"(
    string testUnsupported() {
      string s = "Unknown: \x \z";
      return s;
    }
  )";

  if (!manager.loadScriptSource(source, "test_unsupported.script", errors)) {
    std::cout << "Compilation errors:" << std::endl;
    for (const auto &err : errors) {
      std::cout << "  " << err.toString() << std::endl;
    }
    EXPECT_TRUE(false);
  }

  Value result;
  std::string errorMsg;
  std::vector<Value> args;

  bool success =
      manager.executeProcedure("testUnsupported", args, result, errorMsg);
  EXPECT_TRUE(success);
  std::string str = std::get<std::string>(result);
  std::cout << "  ✓ Unsupported escapes preserved: [" << str << "]"
            << std::endl;
  // Should keep backslash for unsupported sequences
  EXPECT_TRUE(str == "Unknown: \\x \\z");
}

