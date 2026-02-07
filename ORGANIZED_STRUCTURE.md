# CxxScript - Organized Project Structure

## Directory Structure

```
/cxxscript/
│
├── include/              # Public header files (Library API)
│   ├── AST.h            # Abstract Syntax Tree definitions
│   ├── DataTypes.h      # Value types and type system
│   ├── Interpreter.h    # Script interpreter interface
│   ├── Lexer.h          # Lexical analyzer interface
│   ├── Parser.h         # Parser interface
│   ├── ScriptManager.h  # High-level script management API
│   └── Token.h          # Token definitions
│
├── src/                 # Implementation files (Library internals)
│   ├── DataTypes.cpp    # Type system implementation
│   ├── Interpreter.cpp  # Interpreter implementation
│   ├── Lexer.cpp        # Lexer implementation
│   ├── Parser.cpp       # Parser implementation
│   ├── ScriptManager.cpp# ScriptManager implementation
│   └── Token.cpp        # Token implementation
│
├── tests/               # Test files (Comprehensive test suite)
│   ├── test_lexer.cpp              # Lexer unit tests
│   ├── test_parser.cpp             # Parser unit tests
│   ├── test_interpreter.cpp        # Interpreter unit tests
│   ├── test_error_handling.cpp     # Error handling tests
│   ├── test_comprehensive.cpp      # Comprehensive integration tests
│   ├── test_external_functions.cpp # External function binding tests
│   ├── test_external_variables.cpp # External variable binding tests
│   ├── test_multi_file.cpp         # Multi-file script tests
│   ├── test_string_concat.cpp      # String concatenation tests
│   ├── test_escape_sequences.cpp   # String escape handling tests
│   ├── test_control_flow.cpp       # Control flow edge cases
│   ├── test_bitwise.cpp            # Bitwise operator tests
│   └── test_arrays.cpp             # Array behavior and safety
│
├── examples/            # Example applications
│   ├── example_usage.cpp         # Basic usage example
│   ├── demo_error_detection.cpp  # Error detection demo
│   ├── demo_uninitialized.cpp    # Uninitialized variable demo
│   ├── demo_whitespace.cpp       # Whitespace/formatting demo
│   └── test_debug.cpp            # Debug helper example
│
├── scripts/             # Script files
│   ├── example.script            # Basic example script
│   ├── demo_concat.script        # String concatenation demo
│   └── test_files/               # Test script modules
│       ├── validation_rules.script   # User validation rules
│       ├── business_logic.script     # Business logic rules
│       ├── reporting.script          # Reporting functions
│       ├── workflows.script          # High-level workflows
│       ├── math_utils.script         # Math utilities
│       ├── string_utils.script       # String utilities
│       ├── validators.script         # Validation functions
│       └── main_logic.script         # Main logic
│
├── build/               # Build output (generated)
│   ├── lib/            # Compiled library
│   │   └── libCxxScript.a
│   ├── bin/            # Example executables
│   │   ├── example_usage
│   │   ├── demo_error_detection
│   │   ├── demo_uninitialized
│   │   └── demo_whitespace
│   └── tests/          # Test executables
│       ├── test_lexer
│       ├── test_parser
│       ├── test_interpreter
│       ├── test_error_handling
│       ├── test_comprehensive
│       ├── test_external_functions
│       ├── test_multi_file
│       ├── test_string_concat
│       └── test_real_world_app
│
├── CMakeLists.txt       # CMake build configuration
├── README.md            # Project documentation
├── PROJECT_STRUCTURE.md # This file
├── API_CHANGES.md       # API documentation
├── MULTI_FILE_GUIDE.md  # Multi-file scripting guide
└── REAL_WORLD_EXAMPLE.md# Real-world application guide
```

## Build System

### Building the Project

```bash
# Configure and build
cmake -S . -B build
cmake --build build

# Or using traditional make
cd build
cmake ..
make
```

### Build Outputs

- **Library**: `build/lib/libCxxScript.a`
- **Examples**: `build/bin/`
- **Tests**: `build/tests/`

## Running Tests

```bash
# Run all tests
cd build
ctest --output-on-failure

# Run specific test
./tests/test_lexer
./tests/test_parser
./tests/test_real_world_app

# Or from root directory
cd build && ctest
```

## Running Examples

```bash
# Run basic example
./build/bin/example_usage

# Run error detection demo
./build/bin/demo_error_detection

# Or use the custom target
cd build
make run_example
```

## Using the Library

### In Your Project

1. **Link the library**:
   ```cmake
   target_link_libraries(your_app PRIVATE CxxScript)
   ```

2. **Include headers**:
   ```cpp
   #include "ScriptManager.h"
   ```

3. **Use the API**:
   ```cpp
   Script::ScriptManager manager;
   std::vector<Script::CompilationError> errors;
   manager.loadScriptFile("script.script", errors);
   ```

### Installation

```bash
cd build
sudo make install
```

This installs:
- Headers to `/usr/local/include/CxxScript/`
- Library to `/usr/local/lib/`
- Example scripts to `/usr/local/share/CxxScript/scripts/`

## Project Organization Benefits

### 1. **Clean Separation**
- **Library code** (`include/`, `src/`) - Reusable components
- **Test code** (`tests/`) - Quality assurance
- **Examples** (`examples/`) - Documentation and demos
- **Scripts** (`scripts/`) - Business logic and test data

### 2. **Build Organization**
- All build outputs in `build/` directory
- Library in `build/lib/`
- Executables separated: `build/bin/` (examples), `build/tests/` (tests)
- Source tree stays clean

### 3. **Easy Testing**
- All tests in `tests/` directory
- Run with `ctest` from build directory
- Tests reference scripts via relative paths from source root

### 4. **Clear API**
- Public API in `include/` directory
- Implementation details hidden in `src/`
- Easy to identify what users can use

### 5. **Modular Scripts**
- Test scripts organized in `scripts/test_files/`
- Business rules separate from code
- Easy to version control and modify

## Development Workflow

### Adding a New Feature

1. **Add interface** to appropriate header in `include/`
2. **Implement** in corresponding `.cpp` in `src/`
3. **Write tests** in `tests/`
4. **Add example** in `examples/` (optional)
5. **Update documentation**

### Adding a New Test

1. Create `test_*.cpp` in `tests/`
2. Add to `CMakeLists.txt`:
   ```cmake
   add_executable(test_newfeature ${TESTS_DIR}/test_newfeature.cpp)
   target_link_libraries(test_newfeature PRIVATE CxxScript)
   set_target_properties(test_newfeature PROPERTIES
       RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
   )
   add_test(NAME NewFeatureTest COMMAND test_newfeature 
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
   ```

### Adding a Script Module

1. Create `*.script` file in `scripts/` or `scripts/test_files/`
2. Reference it in tests/examples using path relative to source root
3. Tests automatically run from source directory

## CMake Configuration

The `CMakeLists.txt` is organized with:

- **Project setup**: C++17, compiler flags
- **Directory variables**: Clearly defined paths
- **Library target**: Static library with public headers
- **Example targets**: With output to `build/bin/`
- **Test targets**: With output to `build/tests/`
- **CTest integration**: All tests registered
- **Installation rules**: Headers, library, and scripts
- **Configuration output**: Readable project info

## Key Features

✅ **Organized structure** - Clear separation of concerns  
✅ **Build isolation** - All outputs in `build/`  
✅ **Easy testing** - `ctest` integration  
✅ **Clean API** - Public headers separate from implementation  
✅ **Portable** - CMake-based, cross-platform  
✅ **Installable** - Standard installation support  
✅ **Documented** - Multiple documentation files  

## Quick Start Commands

```bash
# Clone and build
git clone <repo>
cd cxxscript
cmake -S . -B build
cmake --build build

# Run tests
cd build && ctest

# Run example
./build/bin/example_usage

# Install (optional)
cd build && sudo make install
```

## Documentation Files

- **README.md** - Main project overview
- **PROJECT_STRUCTURE.md** - This file (structure guide)
- **API_CHANGES.md** - API documentation and usage
- **MULTI_FILE_GUIDE.md** - Multi-file scripting tutorial
- **REAL_WORLD_EXAMPLE.md** - E-commerce business rules example

## Notes

- All paths in tests use relative paths from source root
- CTest runs tests with `WORKING_DIRECTORY` set to source root
- Build directory can be anywhere (out-of-source build)
- Clean rebuild: `rm -rf build && cmake -S . -B build && cmake --build build`
