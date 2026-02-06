#pragma once

#include "DataTypes.h"
#include "Token.h"
#include <memory>
#include <string>
#include <vector>

namespace Script {

// Forward declarations
class ASTNode;
class Expression;
class Statement;

using ASTNodePtr = std::shared_ptr<ASTNode>;
using ExprPtr = std::shared_ptr<Expression>;
using StmtPtr = std::shared_ptr<Statement>;

// Base AST Node
class ASTNode {
public:
  int line;
  int column;

  ASTNode(int ln = 0, int col = 0) : line(ln), column(col) {}
  virtual ~ASTNode() = default;
};

// Expression Nodes
class Expression : public ASTNode {
public:
  using ASTNode::ASTNode;
  virtual ~Expression() = default;
};

class LiteralExpr : public Expression {
public:
  Value value;
  DataType type;

  LiteralExpr(const Value &val, DataType t, int ln = 0, int col = 0)
      : Expression(ln, col), value(val), type(t) {}
};

class VariableExpr : public Expression {
public:
  std::string name;

  VariableExpr(const std::string &n, int ln = 0, int col = 0)
      : Expression(ln, col), name(n) {}
};

class BinaryExpr : public Expression {
public:
  enum class Operator {
    ADD,
    SUBTRACT,
    MULTIPLY,
    DIVIDE,
    MODULO,
    EQUAL,
    NOT_EQUAL,
    LESS_THAN,
    GREATER_THAN,
    LESS_EQUAL,
    GREATER_EQUAL,
    LOGICAL_AND,
    LOGICAL_OR
  };

  ExprPtr left;
  ExprPtr right;
  Operator op;

  BinaryExpr(ExprPtr l, ExprPtr r, Operator o, int ln = 0, int col = 0)
      : Expression(ln, col), left(l), right(r), op(o) {}
};

class UnaryExpr : public Expression {
public:
  enum class Operator { NEGATE, LOGICAL_NOT };

  ExprPtr operand;
  Operator op;

  UnaryExpr(ExprPtr expr, Operator o, int ln = 0, int col = 0)
      : Expression(ln, col), operand(expr), op(o) {}
};

class CallExpr : public Expression {
public:
  std::string functionName;
  std::vector<ExprPtr> arguments;

  CallExpr(const std::string &name, const std::vector<ExprPtr> &args,
           int ln = 0, int col = 0)
      : Expression(ln, col), functionName(name), arguments(args) {}
};

// Statement Nodes
class Statement : public ASTNode {
public:
  using ASTNode::ASTNode;
  virtual ~Statement() = default;
};

class ExpressionStmt : public Statement {
public:
  ExprPtr expression;

  ExpressionStmt(ExprPtr expr, int ln = 0, int col = 0)
      : Statement(ln, col), expression(expr) {}
};

class VarDeclStmt : public Statement {
public:
  DataType type;
  std::string name;
  ExprPtr initializer;

  VarDeclStmt(DataType t, const std::string &n, ExprPtr init, int ln = 0,
              int col = 0)
      : Statement(ln, col), type(t), name(n), initializer(init) {}
};

class AssignStmt : public Statement {
public:
  enum class Operator {
    ASSIGN,
    PLUS_ASSIGN,
    MINUS_ASSIGN,
    MULT_ASSIGN,
    DIV_ASSIGN
  };

  std::string variableName;
  ExprPtr value;
  Operator op;

  AssignStmt(const std::string &var, ExprPtr val, Operator o, int ln = 0,
             int col = 0)
      : Statement(ln, col), variableName(var), value(val), op(o) {}
};

class BlockStmt : public Statement {
public:
  std::vector<StmtPtr> statements;

  BlockStmt(const std::vector<StmtPtr> &stmts, int ln = 0, int col = 0)
      : Statement(ln, col), statements(stmts) {}
};

class IfStmt : public Statement {
public:
  ExprPtr condition;
  StmtPtr thenBranch;
  StmtPtr elseBranch;

  IfStmt(ExprPtr cond, StmtPtr thenB, StmtPtr elseB, int ln = 0, int col = 0)
      : Statement(ln, col), condition(cond), thenBranch(thenB),
        elseBranch(elseB) {}
};

class WhileStmt : public Statement {
public:
  ExprPtr condition;
  StmtPtr body;

  WhileStmt(ExprPtr cond, StmtPtr b, int ln = 0, int col = 0)
      : Statement(ln, col), condition(cond), body(b) {}
};

class ForStmt : public Statement {
public:
  StmtPtr initializer;
  ExprPtr condition;
  StmtPtr increment;
  StmtPtr body;

  ForStmt(StmtPtr init, ExprPtr cond, StmtPtr inc, StmtPtr b, int ln = 0,
          int col = 0)
      : Statement(ln, col), initializer(init), condition(cond), increment(inc),
        body(b) {}
};

class ReturnStmt : public Statement {
public:
  ExprPtr value;

  ReturnStmt(ExprPtr val, int ln = 0, int col = 0)
      : Statement(ln, col), value(val) {}
};

// Procedure Declaration
struct Parameter {
  DataType type;
  std::string name;
};

class ProcedureDecl : public ASTNode {
public:
  DataType returnType;
  std::string name;
  std::vector<Parameter> parameters;
  StmtPtr body;

  ProcedureDecl(DataType retType, const std::string &n,
                const std::vector<Parameter> &params, StmtPtr b, int ln = 0,
                int col = 0)
      : ASTNode(ln, col), returnType(retType), name(n), parameters(params),
        body(b) {}
};

using ProcedureDeclPtr = std::shared_ptr<ProcedureDecl>;

// Script (collection of procedures)
class Script {
public:
  std::string filename;
  std::vector<ProcedureDeclPtr> procedures;

  Script(const std::string &file) : filename(file) {}
};

using ScriptPtr = std::shared_ptr<Script>;

} // namespace Script
