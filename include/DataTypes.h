#pragma once

#include <cstdint>
#include <string>
#include <variant>
#include <stdexcept>

namespace Script {

enum class DataType {
    INT8,
    UINT8,
    INT16,
    UINT16,
    INT32,
    UINT32,
    INT64,
    UINT64,
    STRING,
    BOOL,
    VOID
};

// Variant to hold any script value
using Value = std::variant<
    int8_t,
    uint8_t,
    int16_t,
    uint16_t,
    int32_t,
    uint32_t,
    int64_t,
    uint64_t,
    std::string,
    bool
>;

class ValueHelper {
public:
    static DataType getType(const Value& val);
    static std::string typeToString(DataType type);
    static DataType stringToType(const std::string& str);
    
    // Conversion helpers
    static int64_t toInt64(const Value& val);
    static uint64_t toUInt64(const Value& val);
    static bool toBool(const Value& val);
    static std::string toString(const Value& val);
    
    // Arithmetic operations
    static Value add(const Value& a, const Value& b);
    static Value subtract(const Value& a, const Value& b);
    static Value multiply(const Value& a, const Value& b);
    static Value divide(const Value& a, const Value& b);
    static Value modulo(const Value& a, const Value& b);
    
    // Comparison operations
    static bool greaterThan(const Value& a, const Value& b);
    static bool lessThan(const Value& a, const Value& b);
    static bool greaterOrEqual(const Value& a, const Value& b);
    static bool lessOrEqual(const Value& a, const Value& b);
    static bool equals(const Value& a, const Value& b);
    static bool notEquals(const Value& a, const Value& b);
    
    // Logical operations
    static bool logicalAnd(const Value& a, const Value& b);
    static bool logicalOr(const Value& a, const Value& b);
    static bool logicalNot(const Value& a);
    
    // Create value of specific type
    static Value createValue(DataType type, int64_t intVal);
    static Value createValue(DataType type, uint64_t uintVal);
    static Value createValue(DataType type, const std::string& strVal);
    static Value createValue(DataType type, bool boolVal);
};

} // namespace Script
