#pragma once

#include "Interpreter.h"
#include "Lexer.h"
#include "Parser.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace Script {

struct CompilationError {
  std::string message;
  std::string filename;
  std::string procedureName;
  int line;
  int column;

  CompilationError(const std::string &msg, const std::string &file,
                   const std::string &proc, int ln, int col)
      : message(msg), filename(file), procedureName(proc), line(ln),
        column(col) {}

  std::string toString() const;
};

class ScriptManager {
public:
  ScriptManager();
  ~ScriptManager();

  // Load and compile a script file
  bool loadScriptFile(const std::string &filename,
                      std::vector<CompilationError> &errors);

  // Load and compile script from source code
  bool loadScriptSource(const std::string &source, const std::string &filename,
                        std::vector<CompilationError> &errors);

  // Check if script compiles without errors
  bool checkScript(const std::string &filename,
                   std::vector<CompilationError> &errors);
  bool checkScriptSource(const std::string &source, const std::string &filename,
                         std::vector<CompilationError> &errors);

  // Execute a procedure from any loaded script
  bool executeProcedure(const std::string &procedureName,
                        const std::vector<Value> &arguments, Value &returnValue,
                        std::string &errorMessage);

  // Check if a procedure exists
  bool hasProcedure(const std::string &name) const;

  // Get list of all loaded procedures
  std::vector<std::string> getProcedureNames() const;

  // Get procedure signature information
  struct ProcedureInfo {
    std::string name;
    TypeInfo returnType;
    std::vector<Parameter> parameters;
    std::string filename;
  };

  bool getProcedureInfo(const std::string &name, ProcedureInfo &info) const;

  // Register an external function that can be called from scripts
  void registerExternalFunction(const std::string &name,
                                ExternalFunctionCallback callback);

  // Unregister an external function
  void unregisterExternalFunction(const std::string &name);

  // Check if an external function is registered
  bool hasExternalFunction(const std::string &name) const;

  // Register an external variable that scripts can read/write
  void registerExternalVariable(const std::string &name,
                                ExternalVariableGetter getter,
                                ExternalVariableSetter setter = nullptr);

  // Unregister an external variable
  void unregisterExternalVariable(const std::string &name);

  // Check if an external variable is registered
  bool hasExternalVariable(const std::string &name) const;

  // Clear all loaded scripts
  void clear();

private:
  std::unique_ptr<Interpreter> _interpreter;
  std::unordered_map<std::string, std::string>
      _procedureFiles; // procedure name -> filename

  bool compileScript(const std::string &source, const std::string &filename,
                     std::vector<CompilationError> &errors, bool load);
};

} // namespace Script
