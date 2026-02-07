# CxxScript

CxxScript is a modern C++ scripting engine that lets you write and execute `.script` files with multiple procedures.

## Quick Start

```bash
# Build the project
cmake -S . -B build
cmake --build build

# Run tests
cd build && ctest

# Run example
./build/bin/example_usage
```

## Features

- **Multiple Data Types**: int8, uint8, int16, uint16, int32, uint32, int64, uint64, double, string, bool, and typed arrays of any scalar (e.g., `int32[]`).
- **Arrays Built-ins**: array literals `[1,2,3]`, indexing `arr[0]`, mutation `arr[0] = 5`, `len(arr)`, `push(arr, value)` (returns new length), `pop(arr)` (returns last element, errors on empty).
- **Arithmetic Operators**: +, -, *, /, % with proper precedence (modulo is integer-only; floating point uses +,-,*,/)
- **Bitwise Operators**: &, |, ^, ~, <<, >> (integers only)
- **Logical Operators**: !, &&, || with short-circuit evaluation
- **Control Flow**: if/else, while, for, do-while, switch/case/default, ternary `?:`, break/continue
- **Compound Assignments**: +=, -=, *=, /=
- **Procedure Calls**: Scripts can call other procedures defined in the same or different script files
- **External Function Callbacks**: Call C++ functions from scripts with generic argument passing
- **Comprehensive Error Reporting**: Compilation and runtime errors with line numbers and procedure names
- **Decoupled Parser**: Parser logic is separated for easy unit testing

## Project Structure

```
/cxxscript/
├── include/              # Public headers (Library API)
│   ├── AST.h, DataTypes.h, Interpreter.h
│   ├── Lexer.h, Parser.h, ScriptManager.h, Token.h
├── src/                  # Implementation files
│   ├── DataTypes.cpp, Interpreter.cpp, Lexer.cpp
│   ├── Parser.cpp, ScriptManager.cpp, Token.cpp
├── tests/                # Test suite
│   ├── test_lexer.cpp, test_parser.cpp, test_interpreter.cpp
│   ├── test_error_handling.cpp, test_comprehensive.cpp
│   ├── test_external_functions.cpp, test_multi_file.cpp
│   ├── test_string_concat.cpp, test_real_world_app.cpp
├── examples/             # Example applications
│   ├── example_usage.cpp, demo_error_detection.cpp
├── scripts/              # Script files and test data
│   ├── example.script, demo_concat.script
│   └── test_files/      # Test script modules
├── build/                # Build outputs (generated)
│   ├── lib/             # libCxxScript.a
│   ├── bin/             # Examples
│   └── tests/           # Test executables
├── CMakeLists.txt       # Build configuration
└── Documentation files
    ├── README.md (this file)
    ├── ORGANIZED_STRUCTURE.md   # Detailed structure guide
    ├── API_CHANGES.md           # API documentation
    ├── MULTI_FILE_GUIDE.md      # Multi-file scripting
    └── REAL_WORLD_EXAMPLE.md    # E-commerce example
```

See [ORGANIZED_STRUCTURE.md](ORGANIZED_STRUCTURE.md) for detailed structure documentation.

## Script Syntax

### Procedure Definition

```cpp
returnType procedureName(type1 param1, type2 param2) {
    // statements
    return value;
}
```

### String Literals and Escape Sequences

Strings support the following escape sequences:

```cpp
string examples() {
    string quote = "He said \"Hello\"";           // \" - Quote
    string path = "C:\\Users\\Name";               // \\ - Backslash
    string multiline = "Line 1\nLine 2";          // \n - Newline
    string tabbed = "Col1\tCol2\tCol3";           // \t - Tab
    string cr = "Text\rOverwrite";                // \r - Carriage return
    string nullTerm = "Text\0More";               // \0 - Null character
    
    // Unknown escapes are preserved
    string unknown = "Keep \\x as-is";            // \x - Unknown (kept as \x)
    
    return quote;
}
```

Supported escape sequences:
- `\"` - Double quote
- `\\` - Backslash
- `\n` - Newline
- `\t` - Tab
- `\r` - Carriage return
- `\0` - Null character

### Example Script

#### Arrays

```cpp
int32 arraysDemo(int32 x) {
    int32[] nums = [1, 2, x];
    push(nums, 10);          // nums = [1,2,x,10]
    int32 last = pop(nums);  // last = 10, nums = [1,2,x]
    nums[0] = nums[0] + 5;   // mutate in-place
    return nums[0] + len(nums); // (1+5) + 3 = 9 when x = 3
}
```

```cpp
bool calculate(int32 arg1, int32 arg2) {
    int32 var1 = arg1 + 56;
    int32 var2 = arg2 / 34;
    int32 total = var1 + var2;
    
    if (total > 43) {
        return true;
    }
    
    return false;
}

int32 factorial(int32 n) {
    int32 result = 1;
    for (int32 i = 1; i <= n; i += 1) {
        result *= i;
    }
    return result;
}
```

### Bitwise and Logical Examples

```cpp
// Bitwise operations (integers only)
int32 bitwiseDemo(int32 a, int32 b) {
    int32 andVal = a & b;
    int32 orVal  = a | b;
    int32 xorVal = a ^ b;
    int32 shlVal = a << 1;
    int32 shrVal = b >> 1;
    int32 notVal = ~a;
    return andVal + orVal + xorVal + shlVal + shrVal + notVal;
}

// Logical operators short-circuit
bool shortCircuitDemo(bool x, bool y) {
    // right side is skipped when left decides the outcome
    return (x && expensiveTrue()) || (y || expensiveFalse());
}

// These could be external functions registered from C++
// to observe call counts/side effects.
bool expensiveTrue()  { return true; }
bool expensiveFalse() { return false; }
```

## Usage in C++ Application

```cpp
#include "ScriptManager.h"

using namespace Script;

// 1. Create script manager
ScriptManager scriptManager;

// 2. (Optional) Set up external function callback
scriptManager.setExternalFunctionCallback(
    [](const std::string& name, const std::vector<Value>& args) -> Value {
        if (name == "myFunction") {
            // Handle external function call
            return static_cast<int32_t>(42);
        }
        throw std::runtime_error("Unknown function: " + name);
    }
);

// 3. Load script file
std::vector<CompilationError> errors;
if (!scriptManager.loadScriptFile("example.script", errors)) {
    // Handle compilation errors
    for (const auto& error : errors) {
        std::cout << error.toString() << std::endl;
    }
    return;
}

// 4. Execute a procedure
std::vector<Value> arguments = {
    static_cast<int32_t>(10),
    static_cast<int32_t>(20)
};

Value returnValue;
std::string errorMessage;

if (scriptManager.executeProcedure("calculate", arguments, returnValue, errorMessage)) {
    bool result = std::get<bool>(returnValue);
    std::cout << "Result: " << result << std::endl;
} else {
    std::cout << "Error: " << errorMessage << std::endl;
}
```

## API Reference

### ScriptManager

Main class for managing scripts:

- `loadScriptFile(filename, errors)` - Load and compile a script file
- `loadScriptSource(source, filename, errors)` - Load script from string
- `checkScript(filename, errors)` - Check compilation without loading
- `executeProcedure(name, args, returnValue, errorMsg)` - Execute a procedure
- `hasProcedure(name)` - Check if procedure exists
- `getProcedureNames()` - Get list of all loaded procedures
- `getProcedureInfo(name, info)` - Get procedure signature
- `registerExternalFunction(name, callback)` / `unregisterExternalFunction(name)` - Bind or remove host callbacks callable from scripts
- `clear()` - Clear all loaded scripts

### External Function Callback

```cpp
using ExternalFunctionCallback = std::function<Value(
    const std::string& functionName,
    const std::vector<Value>& arguments
)>;
```

Your C++ application receives:
- `functionName`: Name of the function called from script
- `arguments`: Vector of argument values

Return the result as a `Value`.

## Building and Testing

### Using CMake (Recommended):

```bash
# Create build directory
mkdir build
cd build

# Configure
cmake ..

# Build
cmake --build .

# Run tests
ctest --output-on-failure
# Or use the custom target:
cmake --build . --target run_tests

# Run example
cmake --build . --target run_example
# Or directly:
./example_usage
```

### Manual compilation (if needed):

```bash
# Compile implementation files
g++ -std=c++17 -c Token.cpp DataTypes.cpp Lexer.cpp Parser.cpp Interpreter.cpp ScriptManager.cpp

# Build test executables
g++ -std=c++17 test_lexer.cpp Token.o Lexer.o -o test_lexer
g++ -std=c++17 test_parser.cpp Token.o Lexer.o Parser.o -o test_parser
g++ -std=c++17 test_interpreter.cpp Token.o DataTypes.o Lexer.o Parser.o Interpreter.o ScriptManager.o -o test_interpreter

# Build example
g++ -std=c++17 example_usage.cpp Token.o DataTypes.o Lexer.o Parser.o Interpreter.o ScriptManager.o -o example_usage

# Run tests
./test_lexer
./test_parser
./test_interpreter
./example_usage
```

## Error Handling

### Compilation Errors

Errors detected during parsing include:
- Syntax errors with line and column numbers
- Type mismatches
- Undefined types
- Duplicate procedure names

### Runtime Errors

Errors during execution include:
- Undefined variables
- Division by zero
- Type conversion errors
- Undefined procedure calls
- Line number and procedure name included

## Supported Operations

### Arithmetic
- Addition: `+`
- Subtraction: `-`
- Multiplication: `*`
- Division: `/`
- Modulo: `%`

### Comparison
- Equal: `==`
- Not equal: `!=`
- Less than: `<`
- Greater than: `>`
- Less or equal: `<=`
- Greater or equal: `>=`

### Logical
- AND: `&&`
- OR: `||`
- NOT: `!`

### Assignment
- Assign: `=`
- Plus assign: `+=`
- Minus assign: `-=`
- Multiply assign: `*=`
- Divide assign: `/=`

## Comments

```cpp
// Single-line comment

/*
 * Multi-line
 * comment
 */
```

## License

This is a complete implementation provided for your C++ application.
