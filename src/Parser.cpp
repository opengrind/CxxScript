#include "Parser.h"
#include <sstream>

namespace Script {

Parser::Parser(const std::vector<Token> &tokens, const std::string &filename)
    : _tokens(tokens), _filename(filename), _current(0), _currentProcedure("") {
}

ScriptPtr Parser::parse() {
  auto script = std::make_shared<Script>(_filename);

  while (!isAtEnd()) {
    try {
      auto proc = procedureDeclaration();
      if (proc) {
        script->procedures.push_back(proc);
      }
    } catch (const ParseError &e) {
      // Error already recorded, synchronize and continue
      synchronize();
    }
  }

  return script;
}

bool Parser::isAtEnd() const { return peek().type == TokenType::END_OF_FILE; }

Token Parser::peek() const { return _tokens[_current]; }

Token Parser::previous() const { return _tokens[_current - 1]; }

Token Parser::advance() {
  if (!isAtEnd())
    _current++;
  return previous();
}

bool Parser::check(TokenType type) const {
  if (isAtEnd())
    return false;
  return peek().type == type;
}

bool Parser::match(const std::vector<TokenType> &types) {
  for (TokenType type : types) {
    if (check(type)) {
      advance();
      return true;
    }
  }
  return false;
}

Token Parser::consume(TokenType type, const std::string &message) {
  if (check(type))
    return advance();
  throw error(message);
}

ParseError Parser::error(const std::string &message) {
  Token token = peek();
  std::stringstream ss;
  ss << message << " at line " << token.line << ", column " << token.column;
  if (!_currentProcedure.empty()) {
    ss << " in procedure '" << _currentProcedure << "'";
  }
  return ParseError(ss.str(), token.line, token.column, _currentProcedure);
}

void Parser::synchronize() {
  advance();

  while (!isAtEnd()) {
    if (previous().type == TokenType::SEMICOLON)
      return;
    if (previous().type == TokenType::RBRACE)
      return;

    switch (peek().type) {
    case TokenType::IF:
    case TokenType::WHILE:
    case TokenType::FOR:
    case TokenType::RETURN:
    case TokenType::INT8:
    case TokenType::UINT8:
    case TokenType::INT16:
    case TokenType::UINT16:
    case TokenType::INT32:
    case TokenType::UINT32:
    case TokenType::INT64:
    case TokenType::UINT64:
    case TokenType::STRING:
    case TokenType::BOOL:
    case TokenType::VOID:
      return;
    default:
      break;
    }

    advance();
  }
}

ProcedureDeclPtr Parser::procedureDeclaration() {
  int line = peek().line;
  int column = peek().column;

  DataType returnType = parseType();

  Token name = consume(TokenType::IDENTIFIER, "Expected procedure name");
  _currentProcedure = name.lexeme;

  consume(TokenType::LPAREN, "Expected '(' after procedure name");
  std::vector<Parameter> params = parameters();
  consume(TokenType::RPAREN, "Expected ')' after parameters");

  consume(TokenType::LBRACE, "Expected '{' before procedure body");
  StmtPtr body = block();

  auto proc = std::make_shared<ProcedureDecl>(returnType, name.lexeme, params,
                                              body, line, column);
  _currentProcedure = "";
  return proc;
}

std::vector<Parameter> Parser::parameters() {
  std::vector<Parameter> params;

  if (!check(TokenType::RPAREN)) {
    do {
      DataType type = parseType();
      Token name = consume(TokenType::IDENTIFIER, "Expected parameter name");
      params.push_back({type, name.lexeme});
    } while (match({TokenType::COMMA}));
  }

  return params;
}

DataType Parser::parseType() {
  if (match({TokenType::INT8}))
    return DataType::INT8;
  if (match({TokenType::UINT8}))
    return DataType::UINT8;
  if (match({TokenType::INT16}))
    return DataType::INT16;
  if (match({TokenType::UINT16}))
    return DataType::UINT16;
  if (match({TokenType::INT32}))
    return DataType::INT32;
  if (match({TokenType::UINT32}))
    return DataType::UINT32;
  if (match({TokenType::INT64}))
    return DataType::INT64;
  if (match({TokenType::UINT64}))
    return DataType::UINT64;
  if (match({TokenType::STRING}))
    return DataType::STRING;
  if (match({TokenType::BOOL}))
    return DataType::BOOL;
  if (match({TokenType::VOID}))
    return DataType::VOID;

  throw error("Expected type name");
}

StmtPtr Parser::statement() {
  if (match({TokenType::IF}))
    return ifStatement();
  if (match({TokenType::WHILE}))
    return whileStatement();
  if (match({TokenType::FOR}))
    return forStatement();
  if (match({TokenType::RETURN}))
    return returnStatement();
  if (match({TokenType::LBRACE}))
    return block();

  // Check for variable declaration
  if (check(TokenType::INT8) || check(TokenType::UINT8) ||
      check(TokenType::INT16) || check(TokenType::UINT16) ||
      check(TokenType::INT32) || check(TokenType::UINT32) ||
      check(TokenType::INT64) || check(TokenType::UINT64) ||
      check(TokenType::STRING) || check(TokenType::BOOL)) {
    return varDeclaration();
  }

  return expressionStatement();
}

StmtPtr Parser::varDeclaration() {
  int line = peek().line;
  int column = peek().column;

  DataType type = parseType();
  Token name = consume(TokenType::IDENTIFIER, "Expected variable name");

  ExprPtr initializer = nullptr;
  if (match({TokenType::ASSIGN})) {
    initializer = expression();
  }

  consume(TokenType::SEMICOLON, "Expected ';' after variable declaration");
  return std::make_shared<VarDeclStmt>(type, name.lexeme, initializer, line,
                                       column);
}

StmtPtr Parser::expressionStatement() {
  int line = peek().line;
  int column = peek().column;

  ExprPtr expr = expression();

  // Check for assignment operators
  if (match({TokenType::ASSIGN})) {
    auto varExpr = std::dynamic_pointer_cast<VariableExpr>(expr);
    if (!varExpr) {
      throw error("Invalid assignment target");
    }
    ExprPtr value = expression();
    consume(TokenType::SEMICOLON, "Expected ';' after expression");
    return std::make_shared<AssignStmt>(
        varExpr->name, value, AssignStmt::Operator::ASSIGN, line, column);
  }

  if (match({TokenType::PLUS_ASSIGN})) {
    auto varExpr = std::dynamic_pointer_cast<VariableExpr>(expr);
    if (!varExpr) {
      throw error("Invalid assignment target");
    }
    ExprPtr value = expression();
    consume(TokenType::SEMICOLON, "Expected ';' after expression");
    return std::make_shared<AssignStmt>(
        varExpr->name, value, AssignStmt::Operator::PLUS_ASSIGN, line, column);
  }

  if (match({TokenType::MINUS_ASSIGN})) {
    auto varExpr = std::dynamic_pointer_cast<VariableExpr>(expr);
    if (!varExpr) {
      throw error("Invalid assignment target");
    }
    ExprPtr value = expression();
    consume(TokenType::SEMICOLON, "Expected ';' after expression");
    return std::make_shared<AssignStmt>(
        varExpr->name, value, AssignStmt::Operator::MINUS_ASSIGN, line, column);
  }

  if (match({TokenType::MULT_ASSIGN})) {
    auto varExpr = std::dynamic_pointer_cast<VariableExpr>(expr);
    if (!varExpr) {
      throw error("Invalid assignment target");
    }
    ExprPtr value = expression();
    consume(TokenType::SEMICOLON, "Expected ';' after expression");
    return std::make_shared<AssignStmt>(
        varExpr->name, value, AssignStmt::Operator::MULT_ASSIGN, line, column);
  }

  if (match({TokenType::DIV_ASSIGN})) {
    auto varExpr = std::dynamic_pointer_cast<VariableExpr>(expr);
    if (!varExpr) {
      throw error("Invalid assignment target");
    }
    ExprPtr value = expression();
    consume(TokenType::SEMICOLON, "Expected ';' after expression");
    return std::make_shared<AssignStmt>(
        varExpr->name, value, AssignStmt::Operator::DIV_ASSIGN, line, column);
  }

  consume(TokenType::SEMICOLON, "Expected ';' after expression");
  return std::make_shared<ExpressionStmt>(expr, line, column);
}

StmtPtr Parser::ifStatement() {
  int line = previous().line;
  int column = previous().column;

  consume(TokenType::LPAREN, "Expected '(' after 'if'");
  ExprPtr condition = expression();
  consume(TokenType::RPAREN, "Expected ')' after if condition");

  StmtPtr thenBranch = statement();
  StmtPtr elseBranch = nullptr;

  if (match({TokenType::ELSE})) {
    elseBranch = statement();
  }

  return std::make_shared<IfStmt>(condition, thenBranch, elseBranch, line,
                                  column);
}

StmtPtr Parser::whileStatement() {
  int line = previous().line;
  int column = previous().column;

  consume(TokenType::LPAREN, "Expected '(' after 'while'");
  ExprPtr condition = expression();
  consume(TokenType::RPAREN, "Expected ')' after while condition");

  StmtPtr body = statement();

  return std::make_shared<WhileStmt>(condition, body, line, column);
}

StmtPtr Parser::forStatement() {
  int line = previous().line;
  int column = previous().column;

  consume(TokenType::LPAREN, "Expected '(' after 'for'");

  // Initializer
  StmtPtr initializer = nullptr;
  if (match({TokenType::SEMICOLON})) {
    initializer = nullptr;
  } else if (check(TokenType::INT8) || check(TokenType::UINT8) ||
             check(TokenType::INT16) || check(TokenType::UINT16) ||
             check(TokenType::INT32) || check(TokenType::UINT32) ||
             check(TokenType::INT64) || check(TokenType::UINT64) ||
             check(TokenType::STRING) || check(TokenType::BOOL)) {
    initializer = varDeclaration();
  } else {
    initializer = expressionStatement();
  }

  // Condition
  ExprPtr condition = nullptr;
  if (!check(TokenType::SEMICOLON)) {
    condition = expression();
  }
  consume(TokenType::SEMICOLON, "Expected ';' after loop condition");

  // Increment
  StmtPtr increment = nullptr;
  if (!check(TokenType::RPAREN)) {
    ExprPtr incrementExpr = expression();

    // Check if it's an assignment
    if (match({TokenType::ASSIGN, TokenType::PLUS_ASSIGN,
               TokenType::MINUS_ASSIGN, TokenType::MULT_ASSIGN,
               TokenType::DIV_ASSIGN})) {
      TokenType opType = previous().type;
      auto varExpr = std::dynamic_pointer_cast<VariableExpr>(incrementExpr);
      if (!varExpr) {
        throw error("Invalid assignment target in for loop");
      }
      ExprPtr value = expression();

      AssignStmt::Operator op;
      switch (opType) {
      case TokenType::ASSIGN:
        op = AssignStmt::Operator::ASSIGN;
        break;
      case TokenType::PLUS_ASSIGN:
        op = AssignStmt::Operator::PLUS_ASSIGN;
        break;
      case TokenType::MINUS_ASSIGN:
        op = AssignStmt::Operator::MINUS_ASSIGN;
        break;
      case TokenType::MULT_ASSIGN:
        op = AssignStmt::Operator::MULT_ASSIGN;
        break;
      case TokenType::DIV_ASSIGN:
        op = AssignStmt::Operator::DIV_ASSIGN;
        break;
      default:
        op = AssignStmt::Operator::ASSIGN;
      }
      increment =
          std::make_shared<AssignStmt>(varExpr->name, value, op, line, column);
    } else {
      increment = std::make_shared<ExpressionStmt>(incrementExpr, line, column);
    }
  }

  consume(TokenType::RPAREN, "Expected ')' after for clauses");

  StmtPtr body = statement();

  return std::make_shared<ForStmt>(initializer, condition, increment, body,
                                   line, column);
}

StmtPtr Parser::returnStatement() {
  int line = previous().line;
  int column = previous().column;

  ExprPtr value = nullptr;
  if (!check(TokenType::SEMICOLON)) {
    value = expression();
  }

  consume(TokenType::SEMICOLON, "Expected ';' after return value");
  return std::make_shared<ReturnStmt>(value, line, column);
}

StmtPtr Parser::block() {
  int line = previous().line;
  int column = previous().column;

  std::vector<StmtPtr> statements;

  while (!check(TokenType::RBRACE) && !isAtEnd()) {
    statements.push_back(statement());
  }

  consume(TokenType::RBRACE, "Expected '}' after block");
  return std::make_shared<BlockStmt>(statements, line, column);
}

ExprPtr Parser::expression() { return logicalOr(); }

ExprPtr Parser::logicalOr() {
  ExprPtr expr = logicalAnd();

  while (match({TokenType::OR})) {
    int line = previous().line;
    int column = previous().column;
    ExprPtr right = logicalAnd();
    expr = std::make_shared<BinaryExpr>(
        expr, right, BinaryExpr::Operator::LOGICAL_OR, line, column);
  }

  return expr;
}

ExprPtr Parser::logicalAnd() {
  ExprPtr expr = equality();

  while (match({TokenType::AND})) {
    int line = previous().line;
    int column = previous().column;
    ExprPtr right = equality();
    expr = std::make_shared<BinaryExpr>(
        expr, right, BinaryExpr::Operator::LOGICAL_AND, line, column);
  }

  return expr;
}

ExprPtr Parser::equality() {
  ExprPtr expr = comparison();

  while (match({TokenType::EQUAL, TokenType::NOT_EQUAL})) {
    Token op = previous();
    int line = op.line;
    int column = op.column;
    ExprPtr right = comparison();

    BinaryExpr::Operator binOp = (op.type == TokenType::EQUAL)
                                     ? BinaryExpr::Operator::EQUAL
                                     : BinaryExpr::Operator::NOT_EQUAL;

    expr = std::make_shared<BinaryExpr>(expr, right, binOp, line, column);
  }

  return expr;
}

ExprPtr Parser::comparison() {
  ExprPtr expr = term();

  while (match({TokenType::GREATER_THAN, TokenType::GREATER_EQUAL,
                TokenType::LESS_THAN, TokenType::LESS_EQUAL})) {
    Token op = previous();
    int line = op.line;
    int column = op.column;
    ExprPtr right = term();

    BinaryExpr::Operator binOp;
    switch (op.type) {
    case TokenType::GREATER_THAN:
      binOp = BinaryExpr::Operator::GREATER_THAN;
      break;
    case TokenType::GREATER_EQUAL:
      binOp = BinaryExpr::Operator::GREATER_EQUAL;
      break;
    case TokenType::LESS_THAN:
      binOp = BinaryExpr::Operator::LESS_THAN;
      break;
    case TokenType::LESS_EQUAL:
      binOp = BinaryExpr::Operator::LESS_EQUAL;
      break;
    default:
      binOp = BinaryExpr::Operator::EQUAL;
    }

    expr = std::make_shared<BinaryExpr>(expr, right, binOp, line, column);
  }

  return expr;
}

ExprPtr Parser::term() {
  ExprPtr expr = factor();

  while (match({TokenType::PLUS, TokenType::MINUS})) {
    Token op = previous();
    int line = op.line;
    int column = op.column;
    ExprPtr right = factor();

    BinaryExpr::Operator binOp = (op.type == TokenType::PLUS)
                                     ? BinaryExpr::Operator::ADD
                                     : BinaryExpr::Operator::SUBTRACT;

    expr = std::make_shared<BinaryExpr>(expr, right, binOp, line, column);
  }

  return expr;
}

ExprPtr Parser::factor() {
  ExprPtr expr = unary();

  while (match({TokenType::MULTIPLY, TokenType::DIVIDE, TokenType::MODULO})) {
    Token op = previous();
    int line = op.line;
    int column = op.column;
    ExprPtr right = unary();

    BinaryExpr::Operator binOp;
    switch (op.type) {
    case TokenType::MULTIPLY:
      binOp = BinaryExpr::Operator::MULTIPLY;
      break;
    case TokenType::DIVIDE:
      binOp = BinaryExpr::Operator::DIVIDE;
      break;
    case TokenType::MODULO:
      binOp = BinaryExpr::Operator::MODULO;
      break;
    default:
      binOp = BinaryExpr::Operator::ADD;
    }

    expr = std::make_shared<BinaryExpr>(expr, right, binOp, line, column);
  }

  return expr;
}

ExprPtr Parser::unary() {
  if (match({TokenType::MINUS, TokenType::NOT})) {
    Token op = previous();
    int line = op.line;
    int column = op.column;
    ExprPtr right = unary();

    UnaryExpr::Operator unOp = (op.type == TokenType::MINUS)
                                   ? UnaryExpr::Operator::NEGATE
                                   : UnaryExpr::Operator::LOGICAL_NOT;

    return std::make_shared<UnaryExpr>(right, unOp, line, column);
  }

  return call();
}

ExprPtr Parser::call() {
  ExprPtr expr = primary();

  while (true) {
    if (match({TokenType::LPAREN})) {
      expr = finishCall(expr);
    } else {
      break;
    }
  }

  return expr;
}

ExprPtr Parser::finishCall(ExprPtr callee) {
  auto varExpr = std::dynamic_pointer_cast<VariableExpr>(callee);
  if (!varExpr) {
    throw error("Invalid function call");
  }

  int line = previous().line;
  int column = previous().column;

  std::vector<ExprPtr> arguments;
  if (!check(TokenType::RPAREN)) {
    do {
      arguments.push_back(expression());
    } while (match({TokenType::COMMA}));
  }

  consume(TokenType::RPAREN, "Expected ')' after arguments");

  return std::make_shared<CallExpr>(varExpr->name, arguments, line, column);
}

ExprPtr Parser::primary() {
  if (match({TokenType::TRUE})) {
    return std::make_shared<LiteralExpr>(true, DataType::BOOL, previous().line,
                                         previous().column);
  }

  if (match({TokenType::FALSE})) {
    return std::make_shared<LiteralExpr>(false, DataType::BOOL, previous().line,
                                         previous().column);
  }

  if (match({TokenType::INT_LITERAL})) {
    Token token = previous();
    return std::make_shared<LiteralExpr>(static_cast<int32_t>(token.intValue),
                                         DataType::INT32, token.line,
                                         token.column);
  }

  if (match({TokenType::STRING_LITERAL})) {
    Token token = previous();
    return std::make_shared<LiteralExpr>(token.stringValue, DataType::STRING,
                                         token.line, token.column);
  }

  if (match({TokenType::IDENTIFIER})) {
    Token token = previous();
    return std::make_shared<VariableExpr>(token.lexeme, token.line,
                                          token.column);
  }

  if (match({TokenType::LPAREN})) {
    ExprPtr expr = expression();
    consume(TokenType::RPAREN, "Expected ')' after expression");
    return expr;
  }

  throw error("Expected expression");
}

} // namespace Script
