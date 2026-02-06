#include "Interpreter.h"
#include <sstream>

namespace Script {

// Environment Implementation
void Interpreter::Environment::define(const std::string &name,
                                      const Value &value) {
  if (!_scopes.empty()) {
    _scopes.back()[name] = value;
  } else {
    _globals[name] = value;
  }
}

Value Interpreter::Environment::get(const std::string &name) const {
  // Search from innermost to outermost scope
  for (auto it = _scopes.rbegin(); it != _scopes.rend(); ++it) {
    auto found = it->find(name);
    if (found != it->end()) {
      return found->second;
    }
  }

  // Check globals
  auto found = _globals.find(name);
  if (found != _globals.end()) {
    return found->second;
  }

  // Check parent environment
  if (_parent) {
    return _parent->get(name);
  }

  throw std::runtime_error("Undefined variable: " + name);
}

void Interpreter::Environment::assign(const std::string &name,
                                      const Value &value) {
  // Search from innermost to outermost scope
  for (auto it = _scopes.rbegin(); it != _scopes.rend(); ++it) {
    auto found = it->find(name);
    if (found != it->end()) {
      found->second = value;
      return;
    }
  }

  // Check globals
  auto found = _globals.find(name);
  if (found != _globals.end()) {
    found->second = value;
    return;
  }

  // Check parent environment
  if (_parent) {
    _parent->assign(name, value);
    return;
  }

  throw std::runtime_error("Undefined variable: " + name);
}

bool Interpreter::Environment::has(const std::string &name) const {
  for (auto it = _scopes.rbegin(); it != _scopes.rend(); ++it) {
    if (it->find(name) != it->end()) {
      return true;
    }
  }

  if (_globals.find(name) != _globals.end()) {
    return true;
  }

  if (_parent) {
    return _parent->has(name);
  }

  return false;
}

void Interpreter::Environment::enterScope() { _scopes.emplace_back(); }

void Interpreter::Environment::exitScope() {
  if (!_scopes.empty()) {
    _scopes.pop_back();
  }
}

// Interpreter Implementation
Interpreter::Interpreter()
    : _currentEnv(new Environment()), _currentProcedure("") {}

void Interpreter::registerExternalFunction(const std::string &name,
                                           ExternalFunctionCallback callback) {
  _externalFunctions[name] = callback;
}

void Interpreter::unregisterExternalFunction(const std::string &name) {
  _externalFunctions.erase(name);
}

bool Interpreter::hasExternalFunction(const std::string &name) const {
  return _externalFunctions.find(name) != _externalFunctions.end();
}

void Interpreter::loadScript(ScriptPtr script) {
  for (auto &proc : script->procedures) {
    _procedures[proc->name] = proc;
  }
}

Value Interpreter::executeProcedure(const std::string &name,
                                    const std::vector<Value> &arguments) {
  auto it = _procedures.find(name);
  if (it == _procedures.end()) {
    throw std::runtime_error("Procedure not found: " + name);
  }

  ProcedureDeclPtr proc = it->second;
  _currentProcedure = name;

  // Check argument count
  if (arguments.size() != proc->parameters.size()) {
    std::stringstream ss;
    ss << "Procedure '" << name << "' expects " << proc->parameters.size()
       << " arguments, got " << arguments.size();
    throw runtimeError(ss.str(), proc->line, proc->column);
  }

  // Create new environment for procedure
  Environment procEnv(_currentEnv);
  Environment *previousEnv = _currentEnv;
  _currentEnv = &procEnv;

  // Bind parameters
  for (size_t i = 0; i < proc->parameters.size(); ++i) {
    Value convertedArg = convertToType(arguments[i], proc->parameters[i].type);
    _currentEnv->define(proc->parameters[i].name, convertedArg);
  }

  try {
    execute(proc->body);

    // If we reach here, no return statement was executed
    if (proc->returnType == DataType::VOID) {
      _currentEnv = previousEnv;
      _currentProcedure = "";
      return static_cast<int32_t>(0); // Dummy value
    }

    // Non-void procedure without return
    _currentEnv = previousEnv;
    _currentProcedure = "";
    throw runtimeError("Non-void procedure must return a value", proc->line,
                       proc->column);

  } catch (const ReturnException &ret) {
    _currentEnv = previousEnv;
    _currentProcedure = "";

    if (proc->returnType == DataType::VOID) {
      return static_cast<int32_t>(0); // Dummy value
    }

    return convertToType(ret.value, proc->returnType);
  }
}

bool Interpreter::hasProcedure(const std::string &name) const {
  return _procedures.find(name) != _procedures.end();
}

ProcedureDeclPtr Interpreter::getProcedure(const std::string &name) const {
  auto it = _procedures.find(name);
  if (it != _procedures.end()) {
    return it->second;
  }
  return nullptr;
}

Value Interpreter::evaluate(ExprPtr expr) {
  if (auto *lit = dynamic_cast<LiteralExpr *>(expr.get())) {
    return evaluateLiteral(lit);
  }
  if (auto *var = dynamic_cast<VariableExpr *>(expr.get())) {
    return evaluateVariable(var);
  }
  if (auto *bin = dynamic_cast<BinaryExpr *>(expr.get())) {
    return evaluateBinary(bin);
  }
  if (auto *un = dynamic_cast<UnaryExpr *>(expr.get())) {
    return evaluateUnary(un);
  }
  if (auto *call = dynamic_cast<CallExpr *>(expr.get())) {
    return evaluateCall(call);
  }

  throw runtimeError("Unknown expression type", expr->line, expr->column);
}

void Interpreter::execute(StmtPtr stmt) {
  if (auto *exprStmt = dynamic_cast<ExpressionStmt *>(stmt.get())) {
    executeExpression(exprStmt);
  } else if (auto *varDecl = dynamic_cast<VarDeclStmt *>(stmt.get())) {
    executeVarDecl(varDecl);
  } else if (auto *assign = dynamic_cast<AssignStmt *>(stmt.get())) {
    executeAssign(assign);
  } else if (auto *block = dynamic_cast<BlockStmt *>(stmt.get())) {
    executeBlock(block);
  } else if (auto *ifStmt = dynamic_cast<IfStmt *>(stmt.get())) {
    executeIf(ifStmt);
  } else if (auto *whileStmt = dynamic_cast<WhileStmt *>(stmt.get())) {
    executeWhile(whileStmt);
  } else if (auto *forStmt = dynamic_cast<ForStmt *>(stmt.get())) {
    executeFor(forStmt);
  } else if (auto *retStmt = dynamic_cast<ReturnStmt *>(stmt.get())) {
    executeReturn(retStmt);
  } else {
    throw runtimeError("Unknown statement type", stmt->line, stmt->column);
  }
}

Value Interpreter::evaluateLiteral(LiteralExpr *expr) { return expr->value; }

Value Interpreter::evaluateVariable(VariableExpr *expr) {
  try {
    return _currentEnv->get(expr->name);
  } catch (const std::runtime_error &e) {
    throw runtimeError(e.what(), expr->line, expr->column);
  }
}

Value Interpreter::evaluateBinary(BinaryExpr *expr) {
  Value left = evaluate(expr->left);
  Value right = evaluate(expr->right);

  switch (expr->op) {
  case BinaryExpr::Operator::ADD:
    return ValueHelper::add(left, right);
  case BinaryExpr::Operator::SUBTRACT:
    return ValueHelper::subtract(left, right);
  case BinaryExpr::Operator::MULTIPLY:
    return ValueHelper::multiply(left, right);
  case BinaryExpr::Operator::DIVIDE:
    return ValueHelper::divide(left, right);
  case BinaryExpr::Operator::MODULO:
    return ValueHelper::modulo(left, right);
  case BinaryExpr::Operator::EQUAL:
    return ValueHelper::equals(left, right);
  case BinaryExpr::Operator::NOT_EQUAL:
    return ValueHelper::notEquals(left, right);
  case BinaryExpr::Operator::LESS_THAN:
    return ValueHelper::lessThan(left, right);
  case BinaryExpr::Operator::GREATER_THAN:
    return ValueHelper::greaterThan(left, right);
  case BinaryExpr::Operator::LESS_EQUAL:
    return ValueHelper::lessOrEqual(left, right);
  case BinaryExpr::Operator::GREATER_EQUAL:
    return ValueHelper::greaterOrEqual(left, right);
  case BinaryExpr::Operator::LOGICAL_AND:
    return ValueHelper::logicalAnd(left, right);
  case BinaryExpr::Operator::LOGICAL_OR:
    return ValueHelper::logicalOr(left, right);
  }

  throw runtimeError("Unknown binary operator", expr->line, expr->column);
}

Value Interpreter::evaluateUnary(UnaryExpr *expr) {
  Value operand = evaluate(expr->operand);

  switch (expr->op) {
  case UnaryExpr::Operator::NEGATE:
    return ValueHelper::createValue(DataType::INT32,
                                    -ValueHelper::toInt64(operand));
  case UnaryExpr::Operator::LOGICAL_NOT:
    return ValueHelper::logicalNot(operand);
  }

  throw runtimeError("Unknown unary operator", expr->line, expr->column);
}

Value Interpreter::evaluateCall(CallExpr *expr) {
  // Check if it's a procedure call
  if (hasProcedure(expr->functionName)) {
    std::vector<Value> args;
    for (auto &argExpr : expr->arguments) {
      args.push_back(evaluate(argExpr));
    }
    return executeProcedure(expr->functionName, args);
  }

  // Check if it's a registered external function
  auto extIt = _externalFunctions.find(expr->functionName);
  if (extIt != _externalFunctions.end()) {
    std::vector<Value> args;
    for (auto &argExpr : expr->arguments) {
      args.push_back(evaluate(argExpr));
    }
    return extIt->second(args);
  }

  throw runtimeError("Undefined function: " + expr->functionName, expr->line,
                     expr->column);
}

void Interpreter::executeExpression(ExpressionStmt *stmt) {
  evaluate(stmt->expression);
}

void Interpreter::executeVarDecl(VarDeclStmt *stmt) {
  Value value;

  if (stmt->initializer) {
    value = evaluate(stmt->initializer);
    value = convertToType(value, stmt->type);
  } else {
    // Default initialization
    switch (stmt->type) {
    case DataType::INT8:
      value = static_cast<int8_t>(0);
      break;
    case DataType::UINT8:
      value = static_cast<uint8_t>(0);
      break;
    case DataType::INT16:
      value = static_cast<int16_t>(0);
      break;
    case DataType::UINT16:
      value = static_cast<uint16_t>(0);
      break;
    case DataType::INT32:
      value = static_cast<int32_t>(0);
      break;
    case DataType::UINT32:
      value = static_cast<uint32_t>(0);
      break;
    case DataType::INT64:
      value = static_cast<int64_t>(0);
      break;
    case DataType::UINT64:
      value = static_cast<uint64_t>(0);
      break;
    case DataType::STRING:
      value = std::string("");
      break;
    case DataType::BOOL:
      value = false;
      break;
    default:
      value = static_cast<int32_t>(0);
    }
  }

  _currentEnv->define(stmt->name, value);
}

void Interpreter::executeAssign(AssignStmt *stmt) {
  Value value = evaluate(stmt->value);

  try {
    Value currentValue = _currentEnv->get(stmt->variableName);

    switch (stmt->op) {
    case AssignStmt::Operator::ASSIGN:
      _currentEnv->assign(stmt->variableName, value);
      break;
    case AssignStmt::Operator::PLUS_ASSIGN:
      _currentEnv->assign(stmt->variableName,
                          ValueHelper::add(currentValue, value));
      break;
    case AssignStmt::Operator::MINUS_ASSIGN:
      _currentEnv->assign(stmt->variableName,
                          ValueHelper::subtract(currentValue, value));
      break;
    case AssignStmt::Operator::MULT_ASSIGN:
      _currentEnv->assign(stmt->variableName,
                          ValueHelper::multiply(currentValue, value));
      break;
    case AssignStmt::Operator::DIV_ASSIGN:
      _currentEnv->assign(stmt->variableName,
                          ValueHelper::divide(currentValue, value));
      break;
    }
  } catch (const std::runtime_error &e) {
    throw runtimeError(e.what(), stmt->line, stmt->column);
  }
}

void Interpreter::executeBlock(BlockStmt *stmt) {
  _currentEnv->enterScope();

  try {
    for (auto &statement : stmt->statements) {
      execute(statement);
    }
    _currentEnv->exitScope();
  } catch (...) {
    _currentEnv->exitScope();
    throw;
  }
}

void Interpreter::executeIf(IfStmt *stmt) {
  Value condition = evaluate(stmt->condition);

  if (ValueHelper::toBool(condition)) {
    execute(stmt->thenBranch);
  } else if (stmt->elseBranch) {
    execute(stmt->elseBranch);
  }
}

void Interpreter::executeWhile(WhileStmt *stmt) {
  while (ValueHelper::toBool(evaluate(stmt->condition))) {
    execute(stmt->body);
  }
}

void Interpreter::executeFor(ForStmt *stmt) {
  _currentEnv->enterScope();

  try {
    // Initialize
    if (stmt->initializer) {
      execute(stmt->initializer);
    }

    // Loop
    while (true) {
      // Check condition
      if (stmt->condition && !ValueHelper::toBool(evaluate(stmt->condition))) {
        break;
      }

      // Execute body
      execute(stmt->body);

      // Increment
      if (stmt->increment) {
        execute(stmt->increment);
      }
    }

    _currentEnv->exitScope();
  } catch (...) {
    _currentEnv->exitScope();
    throw;
  }
}

void Interpreter::executeReturn(ReturnStmt *stmt) {
  Value value;

  if (stmt->value) {
    value = evaluate(stmt->value);
  } else {
    value = static_cast<int32_t>(0); // Dummy value for void returns
  }

  throw ReturnException(value);
}

RuntimeError Interpreter::runtimeError(const std::string &message, int line,
                                       int column) {
  return RuntimeError(message, line, column, _currentProcedure);
}

Value Interpreter::convertToType(const Value &val, DataType targetType) {
  DataType sourceType = ValueHelper::getType(val);

  if (sourceType == targetType) {
    return val;
  }

  // Handle conversions
  switch (targetType) {
  case DataType::INT8:
    return ValueHelper::createValue(targetType, ValueHelper::toInt64(val));
  case DataType::INT16:
    return ValueHelper::createValue(targetType, ValueHelper::toInt64(val));
  case DataType::INT32:
    return ValueHelper::createValue(targetType, ValueHelper::toInt64(val));
  case DataType::INT64:
    return ValueHelper::createValue(targetType, ValueHelper::toInt64(val));
  case DataType::UINT8:
    return ValueHelper::createValue(targetType, ValueHelper::toUInt64(val));
  case DataType::UINT16:
    return ValueHelper::createValue(targetType, ValueHelper::toUInt64(val));
  case DataType::UINT32:
    return ValueHelper::createValue(targetType, ValueHelper::toUInt64(val));
  case DataType::UINT64:
    return ValueHelper::createValue(targetType, ValueHelper::toUInt64(val));
  case DataType::BOOL:
    return ValueHelper::createValue(targetType, ValueHelper::toBool(val));
  case DataType::STRING:
    return ValueHelper::createValue(targetType, ValueHelper::toString(val));
  default:
    return val;
  }
}

} // namespace Script
