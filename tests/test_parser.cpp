#include "Lexer.h"
#include "Parser.h"
#include <gtest/gtest.h>

using namespace Script;

TEST(ParserTest, SimpleProcedure) {
  std::string source = R"(
        int32 add(int32 a, int32 b) {
            return a + b;
        }
    )";

  Lexer lexer(source, "test");
  auto tokens = lexer.tokenize();

  Parser parser(tokens, "test");
  auto script = parser.parse();

  ASSERT_EQ(script->procedures.size(), 1);
  EXPECT_EQ(script->procedures[0]->name, "add");
  EXPECT_EQ(script->procedures[0]->returnType, DataType::INT32);
  ASSERT_EQ(script->procedures[0]->parameters.size(), 2);
  EXPECT_EQ(script->procedures[0]->parameters[0].name, "a");
  EXPECT_EQ(script->procedures[0]->parameters[1].name, "b");
}

TEST(ParserTest, VariableDeclaration) {
  std::string source = R"(
        void test() {
            int32 x = 42;
            bool flag = true;
            string msg = "hello";
        }
    )";

  Lexer lexer(source, "test");
  auto tokens = lexer.tokenize();

  Parser parser(tokens, "test");
  auto script = parser.parse();

  ASSERT_EQ(script->procedures.size(), 1);
  auto *block = dynamic_cast<BlockStmt *>(script->procedures[0]->body.get());
  ASSERT_NE(block, nullptr);
  EXPECT_EQ(block->statements.size(), 3);
}

TEST(ParserTest, ControlFlow) {
  std::string source = R"(
        bool test(int32 x) {
            if (x > 10) {
                return true;
            } else {
                return false;
            }
        }
    )";

  Lexer lexer(source, "test");
  auto tokens = lexer.tokenize();

  Parser parser(tokens, "test");
  auto script = parser.parse();

  ASSERT_EQ(script->procedures.size(), 1);
  auto *block = dynamic_cast<BlockStmt *>(script->procedures[0]->body.get());
  ASSERT_NE(block, nullptr);
  ASSERT_EQ(block->statements.size(), 1);

  auto *ifStmt = dynamic_cast<IfStmt *>(block->statements[0].get());
  ASSERT_NE(ifStmt, nullptr);
  EXPECT_NE(ifStmt->elseBranch, nullptr);
}

TEST(ParserTest, Loops) {
  std::string source = R"(
        int32 sum(int32 n) {
            int32 total = 0;
            for (int32 i = 0; i < n; i += 1) {
                total += i;
            }
            return total;
        }
    )";

  Lexer lexer(source, "test");
  auto tokens = lexer.tokenize();

  Parser parser(tokens, "test");
  auto script = parser.parse();

  ASSERT_EQ(script->procedures.size(), 1);
  auto *block = dynamic_cast<BlockStmt *>(script->procedures[0]->body.get());
  ASSERT_NE(block, nullptr);
}

TEST(ParserTest, Expressions) {
  std::string source = R"(
        int32 calculate(int32 a, int32 b) {
            int32 result = (a + b) * 2 - 5 / 3;
            return result;
        }
    )";

  Lexer lexer(source, "test");
  auto tokens = lexer.tokenize();

  Parser parser(tokens, "test");
  auto script = parser.parse();

  ASSERT_EQ(script->procedures.size(), 1);
}
