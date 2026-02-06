# CxxScript - Project Structure

A clean, organized C++ scripting system with comprehensive tests and examples.

## Directory Structure

```
CxxScript/
├── include/              # Public header files (API)
│   ├── AST.h            # Abstract Syntax Tree definitions
│   ├── DataTypes.h      # Value types and helpers
│   ├── Interpreter.h    # Script interpreter
│   ├── Lexer.h          # Lexical analyzer
│   ├── Parser.h         # Syntax parser
│   ├── ScriptManager.h  # High-level script management API
│   └── Token.h          # Token definitions
│
├── src/                 # Library implementation
│   ├── DataTypes.cpp
│   ├── Interpreter.cpp
│   ├── Lexer.cpp
│   ├── Parser.cpp
│   ├── ScriptManager.cpp
│   └── Token.cpp
│
├── tests/               # Unit and integration tests
│   ├── test_lexer.cpp
│   ├── test_parser.cpp
│   ├── test_interpreter.cpp
│   ├── test_error_handling.cpp
│   ├── test_comprehensive.cpp
│   ├── test_external_functions.cpp
│   ├── test_multi_file.cpp
│   ├── test_string_concat.cpp
│   └── test_real_world_app.cpp
│
├── examples/            # Example applications
│   ├── example_usage.cpp
│   └── demo_error_detection.cpp
│
├── scripts/             # Script files
│   ├── test_files/     # Test script files
│   │   ├── validation_rules.script
│   │   ├── business_logic.script
│   │   ├── reporting.script
│   │   ├── workflows.script
│   │   ├── math_utils.script
│   │   ├── string_utils.script
│   │   └── validators.script
│   ├── demo_concat.script
│   └── example.script
│
├── build/              # Build output (generated)
│   ├── lib/           # Library output
│   ├── bin/           # Example binaries
│   └── tests/         # Test binaries
│
├── CMakeLists.txt     # Build configuration
├── README.md          # This file
├── API_CHANGES.md
├── MULTI_FILE_GUIDE.md
└── REAL_WORLD_EXAMPLE.md
```

## Building

### Quick Start

```bash
# Configure
cmake -B build

# Build everything
cmake --build build

# Run all tests
cd build && ctest --output-on-failure
```

### Build Targets

```bash
# Build only the library
cmake --build build --target CxxScript

# Build specific test
cmake --build build --target test_lexer

# Build all tests
cmake --build build --target run_tests

# Build examples
cmake --build build --target example_usage
cmake --build build --target demo_error_detection
```

### Output Locations

After building:
- **Library**: `build/lib/libCxxScript.a`
- **Tests**: `build/tests/test_*`
- **Examples**: `build/bin/example_usage`, `build/bin/demo_error_detection`

## Running Tests

```bash
# From build directory
cd build
ctest --output-on-failure

# Or run specific test
./tests/test_lexer
./tests/test_real_world_app

# Or from root with make
cmake --build build --target run_tests
```

## Running Examples

```bash
# From root directory
./build/bin/example_usage

# Or use make target
cmake --build build --target run_example
```

## Library Usage

### In Your CMake Project

```cmake
# Find the installed library
find_package(CxxScript REQUIRED)

# Link to your target
target_link_libraries(your_app PRIVATE CxxScript::CxxScript)
```

### In Your Code

```cpp
#include <CxxScript/ScriptManager.h>

int main() {
    Script::ScriptManager manager;
    std::vector<Script::CompilationError> errors;
    
    // Load a script
    if (manager.loadScriptFile("path/to/script.script", errors)) {
        // Execute a procedure
        std::vector<Script::Value> args = {
            static_cast<int32_t>(42)
        };
        Script::Value result;
        std::string errorMsg;
        
        if (manager.executeProcedure("myFunction", args, result, errorMsg)) {
            // Success!
        }
    }
    
    return 0;
}
```

## Test Suite

| Test | Description |
|------|-------------|
| **test_lexer** | Tokenization and lexical analysis |
| **test_parser** | Syntax parsing and AST generation |
| **test_interpreter** | Code execution and runtime |
| **test_error_handling** | Error detection and reporting |
| **test_comprehensive** | End-to-end functionality |
| **test_external_functions** | C++ function integration |
| **test_multi_file** | Multiple script file handling |
| **test_string_concat** | String concatenation with all types |
| **test_real_world_app** | Complete e-commerce business rules application |

## Documentation

- **API_CHANGES.md** - Version history and API changes
- **MULTI_FILE_GUIDE.md** - Guide for working with multiple script files
- **REAL_WORLD_EXAMPLE.md** - Detailed real-world application example

## Installation

```bash
# Build and install
cmake -B build -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --build build
sudo cmake --install build

# This installs:
# - Library: /usr/local/lib/libCxxScript.a
# - Headers: /usr/local/include/CxxScript/*.h
# - Scripts: /usr/local/share/CxxScript/scripts/*.script
# - CMake config: /usr/local/lib/cmake/CxxScript/
```

## Development

### Adding a New Test

1. Create test file in `tests/test_mytest.cpp`
2. Add to `CMakeLists.txt`:
```cmake
add_executable(test_mytest ${TESTS_DIR}/test_mytest.cpp)
target_link_libraries(test_mytest PRIVATE CxxScript)
set_target_properties(test_mytest PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
)
add_test(NAME MyTest COMMAND test_mytest WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
```
3. Add to `run_tests` dependencies
4. Rebuild and test

### Adding a New Example

1. Create example in `examples/my_example.cpp`
2. Add to `CMakeLists.txt`:
```cmake
add_executable(my_example ${EXAMPLES_DIR}/my_example.cpp)
target_link_libraries(my_example PRIVATE CxxScript)
set_target_properties(my_example PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
```
3. Rebuild

## Clean Build

```bash
# Remove build artifacts
rm -rf build/

# Reconfigure and rebuild
cmake -B build
cmake --build build
```

## Features

✅ Strong typing (int8-int64, uint8-uint64, bool, string)  
✅ Procedures with parameters and return values  
✅ Control flow (if/else, while loops)  
✅ String concatenation with automatic type conversion  
✅ External C++ function registration  
✅ Multi-file script support  
✅ Comprehensive error handling  
✅ Real-world business rules example  

## License

See project root for license information.
