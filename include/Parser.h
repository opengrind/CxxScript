#pragma once

#include "AST.h"
#include "Lexer.h"
#include "Token.h"
#include <stdexcept>
#include <string>
#include <vector>

namespace Script {

class ParseError : public std::runtime_error {
public:
  int line;
  int column;
  std::string procedureName;

  ParseError(const std::string &message, int ln, int col,
             const std::string &procName = "")
      : std::runtime_error(message), line(ln), column(col),
        procedureName(procName) {}
};

class Parser {
public:
  Parser(const std::vector<Token> &tokens, const std::string &filename = "");

  ScriptPtr parse();

private:
  std::vector<Token> _tokens;
  std::string _filename;
  size_t _current;
  std::string _currentProcedure;

  // Utility methods
  bool isAtEnd() const;
  Token peek() const;
  Token previous() const;
  Token advance();
  bool check(TokenType type) const;
  bool match(const std::vector<TokenType> &types);
  Token consume(TokenType type, const std::string &message);

  ParseError error(const std::string &message);
  void synchronize();

  // Parsing methods
  ProcedureDeclPtr procedureDeclaration();
  std::vector<Parameter> parameters();
  DataType parseType();

  StmtPtr statement();
  StmtPtr varDeclaration();
  StmtPtr expressionStatement();
  StmtPtr ifStatement();
  StmtPtr whileStatement();
  StmtPtr forStatement();
  StmtPtr returnStatement();
  StmtPtr block();

  ExprPtr expression();
  ExprPtr assignment();
  ExprPtr logicalOr();
  ExprPtr logicalAnd();
  ExprPtr equality();
  ExprPtr comparison();
  ExprPtr term();
  ExprPtr factor();
  ExprPtr unary();
  ExprPtr primary();
  ExprPtr call();
  ExprPtr finishCall(ExprPtr callee);
};

} // namespace Script
