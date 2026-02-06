#include "Lexer.h"
#include <gtest/gtest.h>

using namespace Script;

TEST(LexerTest, BasicTokens) {
  Lexer lexer("int32 uint64 bool string void", "test");
  auto tokens = lexer.tokenize();

  ASSERT_EQ(tokens.size(), 6); // 5 tokens + EOF
  EXPECT_EQ(tokens[0].type, TokenType::INT32);
  EXPECT_EQ(tokens[1].type, TokenType::UINT64);
  EXPECT_EQ(tokens[2].type, TokenType::BOOL);
  EXPECT_EQ(tokens[3].type, TokenType::STRING);
  EXPECT_EQ(tokens[4].type, TokenType::VOID);
}

TEST(LexerTest, Operators) {
  Lexer lexer("+ - * / % = += -= *= /= == != < > <= >= && || !", "test");
  auto tokens = lexer.tokenize();

  EXPECT_EQ(tokens[0].type, TokenType::PLUS);
  EXPECT_EQ(tokens[1].type, TokenType::MINUS);
  EXPECT_EQ(tokens[2].type, TokenType::MULTIPLY);
  EXPECT_EQ(tokens[3].type, TokenType::DIVIDE);
  EXPECT_EQ(tokens[4].type, TokenType::MODULO);
  EXPECT_EQ(tokens[5].type, TokenType::ASSIGN);
  EXPECT_EQ(tokens[6].type, TokenType::PLUS_ASSIGN);
  EXPECT_EQ(tokens[7].type, TokenType::MINUS_ASSIGN);
  EXPECT_EQ(tokens[8].type, TokenType::MULT_ASSIGN);
  EXPECT_EQ(tokens[9].type, TokenType::DIV_ASSIGN);
  EXPECT_EQ(tokens[10].type, TokenType::EQUAL);
  EXPECT_EQ(tokens[11].type, TokenType::NOT_EQUAL);
  EXPECT_EQ(tokens[12].type, TokenType::LESS_THAN);
  EXPECT_EQ(tokens[13].type, TokenType::GREATER_THAN);
  EXPECT_EQ(tokens[14].type, TokenType::LESS_EQUAL);
  EXPECT_EQ(tokens[15].type, TokenType::GREATER_EQUAL);
  EXPECT_EQ(tokens[16].type, TokenType::AND);
  EXPECT_EQ(tokens[17].type, TokenType::OR);
  EXPECT_EQ(tokens[18].type, TokenType::NOT);
}

TEST(LexerTest, Literals) {
  Lexer lexer("42 \"hello world\" true false", "test");
  auto tokens = lexer.tokenize();

  EXPECT_EQ(tokens[0].type, TokenType::INT_LITERAL);
  EXPECT_EQ(tokens[0].intValue, 42);

  EXPECT_EQ(tokens[1].type, TokenType::STRING_LITERAL);
  EXPECT_EQ(tokens[1].stringValue, "hello world");

  EXPECT_EQ(tokens[2].type, TokenType::TRUE);
  EXPECT_EQ(tokens[3].type, TokenType::FALSE);
}

TEST(LexerTest, Keywords) {
  Lexer lexer("if else while for return", "test");
  auto tokens = lexer.tokenize();

  EXPECT_EQ(tokens[0].type, TokenType::IF);
  EXPECT_EQ(tokens[1].type, TokenType::ELSE);
  EXPECT_EQ(tokens[2].type, TokenType::WHILE);
  EXPECT_EQ(tokens[3].type, TokenType::FOR);
  EXPECT_EQ(tokens[4].type, TokenType::RETURN);
}

TEST(LexerTest, Comments) {
  Lexer lexer("int32 // line comment\nint64 /* block comment */ bool", "test");
  auto tokens = lexer.tokenize();

  ASSERT_EQ(tokens.size(), 4); // 3 tokens + EOF
  EXPECT_EQ(tokens[0].type, TokenType::INT32);
  EXPECT_EQ(tokens[1].type, TokenType::INT64);
  EXPECT_EQ(tokens[2].type, TokenType::BOOL);
}
