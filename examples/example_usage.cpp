#include "ScriptManager.h"
#include <iostream>
#include <vector>

using namespace Script;

int main() {
  std::cout << "=== CxxScript Example ===" << std::endl;

  // Create script manager
  ScriptManager scriptManager;

  // Register external functions
  scriptManager.registerExternalFunction(
      "print", [](const std::vector<Value> &args) -> Value {
        std::cout << "External function called: print" << std::endl;
        for (const auto &arg : args) {
          std::cout << "  " << ValueHelper::toString(arg) << std::endl;
        }
        return static_cast<int32_t>(0);
      });

  scriptManager.registerExternalFunction(
      "getSystemValue", [](const std::vector<Value> & /*args*/) -> Value {
        std::cout << "External function called: getSystemValue" << std::endl;
        return static_cast<int32_t>(42);
      });

  scriptManager.registerExternalFunction(
      "multiply", [](const std::vector<Value> &args) -> Value {
        std::cout << "External function called: multiply" << std::endl;
        if (args.size() != 2) {
          throw std::runtime_error("multiply expects 2 arguments");
        }
        int32_t a = ValueHelper::toInt64(args[0]);
        int32_t b = ValueHelper::toInt64(args[1]);
        return a * b;
      });

  // Load a script file
  std::cout << "\n1. Loading script file..." << std::endl;
  std::vector<CompilationError> errors;

  if (!scriptManager.loadScriptFile("example.script", errors)) {
    std::cout << "Failed to load script file!" << std::endl;
    for (const auto &error : errors) {
      std::cout << "  " << error.toString() << std::endl;
    }
    return 1;
  }

  std::cout << "   Script loaded successfully!" << std::endl;

  // List all loaded procedures
  std::cout << "\n2. Loaded procedures:" << std::endl;
  auto procNames = scriptManager.getProcedureNames();
  for (const auto &name : procNames) {
    ScriptManager::ProcedureInfo info;
    if (scriptManager.getProcedureInfo(name, info)) {
      std::cout << "   - " << ValueHelper::typeToString(info.returnType) << " "
                << info.name << "(";
      for (size_t i = 0; i < info.parameters.size(); ++i) {
        if (i > 0)
          std::cout << ", ";
        std::cout << ValueHelper::typeToString(info.parameters[i].type) << " "
                  << info.parameters[i].name;
      }
      std::cout << ")" << std::endl;
    }
  }

  // Execute procedures and display results
  std::cout << "\n3. Executing procedures:" << std::endl;

  // Test factorial
  {
    std::vector<Value> args = {static_cast<int32_t>(5)};
    Value result;
    std::string errorMsg;

    if (scriptManager.executeProcedure("factorial", args, result, errorMsg)) {
      std::cout << "   factorial(5) = " << std::get<int32_t>(result)
                << std::endl;
    } else {
      std::cout << "   Error: " << errorMsg << std::endl;
    }
  }

  // Test isPrime
  {
    std::vector<Value> args = {static_cast<int32_t>(17)};
    Value result;
    std::string errorMsg;

    if (scriptManager.executeProcedure("isPrime", args, result, errorMsg)) {
      std::cout << "   isPrime(17) = "
                << (std::get<bool>(result) ? "true" : "false") << std::endl;
    } else {
      std::cout << "   Error: " << errorMsg << std::endl;
    }
  }

  // Test calculate (from requirements)
  {
    std::vector<Value> args = {static_cast<int32_t>(10),
                               static_cast<int32_t>(100)};
    Value result;
    std::string errorMsg;

    if (scriptManager.executeProcedure("calculate", args, result, errorMsg)) {
      std::cout << "   calculate(10, 100) = "
                << (std::get<bool>(result) ? "true" : "false") << std::endl;
    } else {
      std::cout << "   Error: " << errorMsg << std::endl;
    }
  }

  // Test fibonacci
  {
    std::vector<Value> args = {static_cast<int32_t>(10)};
    Value result;
    std::string errorMsg;

    if (scriptManager.executeProcedure("fibonacci", args, result, errorMsg)) {
      std::cout << "   fibonacci(10) = " << std::get<int32_t>(result)
                << std::endl;
    } else {
      std::cout << "   Error: " << errorMsg << std::endl;
    }
  }

  // Test string function
  {
    std::vector<Value> args = {std::string("World")};
    Value result;
    std::string errorMsg;

    if (scriptManager.executeProcedure("greet", args, result, errorMsg)) {
      std::cout << "   greet(\"World\") = \"" << std::get<std::string>(result)
                << "\"" << std::endl;
    } else {
      std::cout << "   Error: " << errorMsg << std::endl;
    }
  }

  // Load and test a script with external function calls
  std::cout << "\n4. Testing external function calls:" << std::endl;

  std::string externalTestScript = R"(
        int32 testExternal() {
            int32 sysValue = getSystemValue();
            int32 doubled = multiply(sysValue, 2);
            return doubled;
        }
    )";

  errors.clear();
  if (scriptManager.loadScriptSource(externalTestScript, "external_test.script",
                                     errors)) {
    std::vector<Value> args;
    Value result;
    std::string errorMsg;

    if (scriptManager.executeProcedure("testExternal", args, result,
                                       errorMsg)) {
      std::cout << "   testExternal() = " << std::get<int32_t>(result)
                << std::endl;
    } else {
      std::cout << "   Error: " << errorMsg << std::endl;
    }
  }

  // Test error handling
  std::cout << "\n5. Testing error handling:" << std::endl;

  std::string errorScript = R"(
        int32 badProcedure() {
            int32 x = undefinedVariable;
            return x;
        }
    )";

  errors.clear();
  if (scriptManager.loadScriptSource(errorScript, "error_test.script",
                                     errors)) {
    std::vector<Value> args;
    Value result;
    std::string errorMsg;

    if (!scriptManager.executeProcedure("badProcedure", args, result,
                                        errorMsg)) {
      std::cout << "   Caught runtime error: " << errorMsg << std::endl;
    }
  }

  // Test compile-time error detection
  std::cout << "\n6. Testing compile-time error detection:" << std::endl;

  std::string syntaxErrorScript = R"(
        int32 syntaxError( {
            return 42
        }
    )";

  errors.clear();
  if (!scriptManager.checkScriptSource(syntaxErrorScript, "syntax_error.script",
                                       errors)) {
    std::cout << "   Detected compilation errors:" << std::endl;
    for (const auto &error : errors) {
      std::cout << "     " << error.toString() << std::endl;
    }
  }

  std::cout << "\n=== Example completed successfully! ===" << std::endl;

  return 0;
}
