#include "ScriptManager.h"
#include <fstream>
#include <sstream>
#include <utility>

namespace Script {

std::string CompilationError::toString() const {
  std::stringstream ss;
  ss << filename << ":" << line << ":" << column << ": error: " << message;
  if (!procedureName.empty()) {
    ss << " in procedure '" << procedureName << "'";
  }
  return ss.str();
}

ScriptManager::ScriptManager()
    : _interpreter(std::make_unique<Interpreter>()) {}

ScriptManager::~ScriptManager() = default;

bool ScriptManager::loadScriptFile(const std::string &filename,
                                   std::vector<CompilationError> &errors) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    errors.push_back(
        CompilationError("Failed to open file", filename, "", 0, 0));
    return false;
  }

  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string source = buffer.str();

  return loadScriptSource(source, filename, errors);
}

bool ScriptManager::loadScriptSource(const std::string &source,
                                     const std::string &filename,
                                     std::vector<CompilationError> &errors) {
  return compileScript(source, filename, errors, true);
}

bool ScriptManager::checkScript(const std::string &filename,
                                std::vector<CompilationError> &errors) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    errors.push_back(
        CompilationError("Failed to open file", filename, "", 0, 0));
    return false;
  }

  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string source = buffer.str();

  return checkScriptSource(source, filename, errors);
}

bool ScriptManager::checkScriptSource(const std::string &source,
                                      const std::string &filename,
                                      std::vector<CompilationError> &errors) {
  return compileScript(source, filename, errors, false);
}

bool ScriptManager::compileScript(const std::string &source,
                                  const std::string &filename,
                                  std::vector<CompilationError> &errors,
                                  bool load) {
  errors.clear();

  try {
    // Tokenize
    Lexer lexer(source, filename);
    std::vector<Token> tokens;

    try {
      tokens = lexer.tokenize();
    } catch (const std::exception &e) {
      errors.push_back(CompilationError(e.what(), filename, "", 0, 0));
      return false;
    }

    // Check for unknown tokens
    for (const auto &token : tokens) {
      if (token.type == TokenType::UNKNOWN) {
        std::stringstream ss;
        ss << "Unexpected character: '" << token.lexeme << "'";
        errors.push_back(
            CompilationError(ss.str(), filename, "", token.line, token.column));
      }
    }

    if (!errors.empty()) {
      return false;
    }

    // Parse
    Parser parser(tokens, filename);
    ScriptPtr script;

    try {
      script = parser.parse();
    } catch (const ParseError &e) {
      errors.push_back(CompilationError(e.what(), filename, e.procedureName,
                                        e.line, e.column));
      return false;
    } catch (const std::exception &e) {
      errors.push_back(CompilationError(e.what(), filename, "", 0, 0));
      return false;
    }

    if (parser.hasErrors()) {
      for (const auto &pe : parser.getErrors()) {
        errors.push_back(CompilationError(pe.what(), filename, pe.procedureName,
                                          pe.line, pe.column));
      }
      return false;
    }

    // Check for procedures with duplicate names
    std::unordered_map<std::string, int> procNames;
    for (const auto &proc : script->procedures) {
      if (procNames.find(proc->name) != procNames.end()) {
        errors.push_back(
            CompilationError("Duplicate procedure name: " + proc->name,
                             filename, proc->name, proc->line, proc->column));
      }
      procNames[proc->name]++;
    }

    if (!errors.empty()) {
      return false;
    }

    // Load into interpreter if requested
    if (load) {
      _interpreter->loadScript(script);

      // Track which file each procedure came from
      for (const auto &proc : script->procedures) {
        _procedureFiles[proc->name] = filename;
      }
    }

    return true;

  } catch (const std::exception &e) {
    errors.push_back(CompilationError(e.what(), filename, "", 0, 0));
    return false;
  }
}

bool ScriptManager::executeProcedure(const std::string &procedureName,
                                     const std::vector<Value> &arguments,
                                     Value &returnValue,
                                     std::string &errorMessage) {
  try {
    returnValue = _interpreter->executeProcedure(procedureName, arguments);
    return true;
  } catch (const RuntimeError &e) {
    std::stringstream ss;
    ss << "Runtime error at line " << e.line << ", column " << e.column;
    if (!e.procedureName.empty()) {
      ss << " in procedure '" << e.procedureName << "'";
    }
    ss << ": " << e.what();
    errorMessage = ss.str();
    return false;
  } catch (const std::exception &e) {
    errorMessage = std::string("Runtime error: ") + e.what();
    return false;
  }
}

bool ScriptManager::hasProcedure(const std::string &name) const {
  return _interpreter->hasProcedure(name);
}

std::vector<std::string> ScriptManager::getProcedureNames() const {
  std::vector<std::string> names;
  for (const auto &pair : _procedureFiles) {
    names.push_back(pair.first);
  }
  return names;
}

bool ScriptManager::getProcedureInfo(const std::string &name,
                                     ProcedureInfo &info) const {
  auto proc = _interpreter->getProcedure(name);
  if (!proc) {
    return false;
  }

  info.name = proc->name;
  info.returnType = proc->returnType;
  info.parameters = proc->parameters;

  auto it = _procedureFiles.find(name);
  if (it != _procedureFiles.end()) {
    info.filename = it->second;
  }

  return true;
}

void ScriptManager::registerExternalFunction(
    const std::string &name, ExternalFunctionCallback callback) {
  _interpreter->registerExternalFunction(name, callback);
}

void ScriptManager::registerExternalFunctions(
    const std::vector<ExternalBinding> &bindings) {
  _interpreter->registerExternalFunctions(bindings);
}

void ScriptManager::registerExternalFunctions(
    std::initializer_list<ExternalBinding> bindings) {
  _interpreter->registerExternalFunctions(bindings);
}

void ScriptManager::unregisterExternalFunction(const std::string &name) {
  _interpreter->unregisterExternalFunction(name);
}

bool ScriptManager::hasExternalFunction(const std::string &name) const {
  return _interpreter->hasExternalFunction(name);
}

void ScriptManager::registerExternalVariable(
    const std::string &name, ExternalVariableGetter getter,
    ExternalVariableSetter setter) {
  _interpreter->registerExternalVariable(name, std::move(getter),
                                         std::move(setter));
}

void ScriptManager::registerExternalVariableReadOnly(
    const std::string &name, ExternalVariableGetter getter) {
  _interpreter->registerExternalVariableReadOnly(name, std::move(getter));
}

void ScriptManager::unregisterExternalVariable(const std::string &name) {
  _interpreter->unregisterExternalVariable(name);
}

bool ScriptManager::hasExternalVariable(const std::string &name) const {
  return _interpreter->hasExternalVariable(name);
}

void ScriptManager::clear() {
  _interpreter = std::make_unique<Interpreter>();
  _procedureFiles.clear();
}

} // namespace Script
