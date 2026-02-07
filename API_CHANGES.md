# External Function API Changes

## Summary
Changed from a single callback function to a registry-based approach that supports multiple named external functions.

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
void unregisterExternalFunction(const std::string &name);
bool hasExternalFunction(const std::string &name) const;
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
```

## External Variables (New)

Expose host values to scripts with dedicated getters/setters.

```cpp
using ExternalVariableGetter = std::function<Value()>;
using ExternalVariableSetter = std::function<void(const Value&)>; // optional

void registerExternalVariable(const std::string &name,
                              ExternalVariableGetter getter,
                              ExternalVariableSetter setter = nullptr);
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

New test file: `test_external_functions.cpp`
- ✅ Multiple external function registration
- ✅ Function overwrite behavior
- ✅ Unregister functionality
- ✅ Mixed internal and external calls
- ✅ Different return types (int32, string, bool, double)
- ✅ Array arguments/returns

New test file: `test_external_variables.cpp`
- ✅ Read/write external variable bridge
- ✅ Read-only protection on setter-less variables
- ✅ Compound assignments on external values
- ✅ Coverage for string, bool, double, array variables

All existing tests updated and passing (6/6 test suites).
