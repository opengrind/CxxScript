# External Function API Changes

## Summary
Changed from a single callback function to a registry-based approach that supports multiple named external functions, bulk registration helpers, typed wrappers, and read-only external variables.

## API Changes

### Old API (Single Callback)
```cpp
// Old signature - single callback for all external functions
using ExternalFunctionCallback =
    std::function<Value(const std::string &functionName, 
                       const std::vector<Value> &args)>;

// ScriptManager API
void setExternalFunctionCallback(ExternalFunctionCallback callback);
```

**Usage Example:**
```cpp
manager.setExternalFunctionCallback(
    [](const std::string &functionName, const std::vector<Value> &args) -> Value {
        if (functionName == "add") {
            return std::get<int32_t>(args[0]) + std::get<int32_t>(args[1]);
        }
        if (functionName == "multiply") {
            return std::get<int32_t>(args[0]) * std::get<int32_t>(args[1]);
        }
        throw std::runtime_error("Unknown function: " + functionName);
    });
```

### New API (Multiple Named Functions)
```cpp
// New signature - individual function callbacks
using ExternalFunctionCallback =
    std::function<Value(const std::vector<Value> &args)>;

// ScriptManager API
void registerExternalFunction(const std::string &name,
                              ExternalFunctionCallback callback);
// Bulk registration (vector or initializer_list)
void registerExternalFunctions(const std::vector<ExternalBinding> &bindings);
void registerExternalFunctions(std::initializer_list<ExternalBinding> bindings);
void unregisterExternalFunction(const std::string &name);
bool hasExternalFunction(const std::string &name) const;

// Typed helpers (int32_t, double, bool, string)
template <typename Ret, typename Arg>
void registerExternalFunctionUnary(const std::string &name,
                                   std::function<Ret(Arg)> fn);

template <typename Ret, typename Arg1, typename Arg2>
void registerExternalFunctionBinary(const std::string &name,
                                    std::function<Ret(Arg1, Arg2)> fn);
```

**Usage Example:**
```cpp
// Register individual functions
manager.registerExternalFunction("add",
    [](const std::vector<Value> &args) -> Value {
        return std::get<int32_t>(args[0]) + std::get<int32_t>(args[1]);
    });

manager.registerExternalFunction("multiply",
    [](const std::vector<Value> &args) -> Value {
        return std::get<int32_t>(args[0]) * std::get<int32_t>(args[1]);
    });

// Check if function exists
if (manager.hasExternalFunction("add")) {
    // ...
}

// Unregister a function
manager.unregisterExternalFunction("add");

// Bulk register with initializer_list
manager.registerExternalFunctions({
    {"triple", [](const std::vector<Value> &args) {
        return static_cast<int32_t>(ValueHelper::toInt64(args[0]) * 3);
    }},
    {"concat", [](const std::vector<Value> &args) {
        return std::get<std::string>(args[0]) + std::get<std::string>(args[1]);
    }},
});

// Typed helper (binary int32)
manager.registerExternalFunctionBinary<int32_t, int32_t, int32_t>(
    "add", [](int32_t a, int32_t b) { return a + b; });
```

## External Variables (New)

Expose host values to scripts with dedicated getters/setters.

```cpp
using ExternalVariableGetter = std::function<Value()>;
using ExternalVariableSetter = std::function<void(const Value&)>; // optional

void registerExternalVariable(const std::string &name,
                              ExternalVariableGetter getter,
                              ExternalVariableSetter setter = nullptr);
// Convenience for read-only variables
void registerExternalVariableReadOnly(const std::string &name,
                                       ExternalVariableGetter getter);
void unregisterExternalVariable(const std::string &name);
bool hasExternalVariable(const std::string &name) const;
```

**Usage Example:**

```cpp
int32_t sharedCounter = 10;
manager.registerExternalVariable(
    "counter",
    [&]() -> Value { return static_cast<int32_t>(sharedCounter); },
    [&](const Value &v) { sharedCounter = std::get<int32_t>(v); });

// In script: counter += 5; return counter; // updates sharedCounter
```

## Benefits

1. **Cleaner Code**: Each function has its own lambda/callback without needing a dispatcher
2. **Better Performance**: Direct lookup by name instead of if-else chain
3. **Dynamic Management**: Can add/remove functions at runtime
4. **Type Safety**: Each function callback is independent
5. **Easier Testing**: Can register different implementations for testing
6. **Overwrite Support**: Re-registering a function replaces the previous implementation
7. **Ergonomics**: Bulk/initializer_list registration and typed helpers reduce boilerplate

## Migration Guide

**Before:**
```cpp
manager.setExternalFunctionCallback(
    [](const std::string &name, const std::vector<Value> &args) -> Value {
        if (name == "func1") { /* ... */ }
        if (name == "func2") { /* ... */ }
        throw std::runtime_error("Unknown: " + name);
    });
```

**After:**
```cpp
manager.registerExternalFunction("func1",
    [](const std::vector<Value> &args) -> Value { /* ... */ });

manager.registerExternalFunction("func2",
    [](const std::vector<Value> &args) -> Value { /* ... */ });
```

## Test Coverage

`tests/test_external_functions.cpp`
- ✅ Multiple external function registration
- ✅ Bulk registration (vector and initializer_list)
- ✅ Typed helpers (unary/binary, int32)
- ✅ Function overwrite and unregister
- ✅ Mixed internal and external calls
- ✅ Different return types (int32, string, bool, double)
- ✅ Array arguments/returns

`tests/test_external_variables.cpp`
- ✅ Read/write and read-only external variables
- ✅ Compound assignments on external values
- ✅ Coverage for string, bool, double, array variables

Full suite passing (113 tests).
