#include "DataTypes.h"
#include <stdexcept>

namespace Script {

DataType ValueHelper::getType(const Value &val) {
  if (std::holds_alternative<int8_t>(val))
    return DataType::INT8;
  if (std::holds_alternative<uint8_t>(val))
    return DataType::UINT8;
  if (std::holds_alternative<int16_t>(val))
    return DataType::INT16;
  if (std::holds_alternative<uint16_t>(val))
    return DataType::UINT16;
  if (std::holds_alternative<int32_t>(val))
    return DataType::INT32;
  if (std::holds_alternative<uint32_t>(val))
    return DataType::UINT32;
  if (std::holds_alternative<int64_t>(val))
    return DataType::INT64;
  if (std::holds_alternative<uint64_t>(val))
    return DataType::UINT64;
  if (std::holds_alternative<std::string>(val))
    return DataType::STRING;
  if (std::holds_alternative<bool>(val))
    return DataType::BOOL;
  return DataType::VOID;
}

std::string ValueHelper::typeToString(DataType type) {
  switch (type) {
  case DataType::INT8:
    return "int8";
  case DataType::UINT8:
    return "uint8";
  case DataType::INT16:
    return "int16";
  case DataType::UINT16:
    return "uint16";
  case DataType::INT32:
    return "int32";
  case DataType::UINT32:
    return "uint32";
  case DataType::INT64:
    return "int64";
  case DataType::UINT64:
    return "uint64";
  case DataType::STRING:
    return "string";
  case DataType::BOOL:
    return "bool";
  case DataType::VOID:
    return "void";
  }
  return "unknown";
}

DataType ValueHelper::stringToType(const std::string &str) {
  if (str == "int8")
    return DataType::INT8;
  if (str == "uint8")
    return DataType::UINT8;
  if (str == "int16")
    return DataType::INT16;
  if (str == "uint16")
    return DataType::UINT16;
  if (str == "int32")
    return DataType::INT32;
  if (str == "uint32")
    return DataType::UINT32;
  if (str == "int64")
    return DataType::INT64;
  if (str == "uint64")
    return DataType::UINT64;
  if (str == "string")
    return DataType::STRING;
  if (str == "bool")
    return DataType::BOOL;
  if (str == "void")
    return DataType::VOID;
  throw std::runtime_error("Unknown type: " + str);
}

int64_t ValueHelper::toInt64(const Value &val) {
  return std::visit(
      [](auto &&arg) -> int64_t {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
          throw std::runtime_error("Cannot convert string to int64");
        } else if constexpr (std::is_same_v<T, bool>) {
          return arg ? 1 : 0;
        } else {
          return static_cast<int64_t>(arg);
        }
      },
      val);
}

uint64_t ValueHelper::toUInt64(const Value &val) {
  return std::visit(
      [](auto &&arg) -> uint64_t {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
          throw std::runtime_error("Cannot convert string to uint64");
        } else if constexpr (std::is_same_v<T, bool>) {
          return arg ? 1 : 0;
        } else {
          return static_cast<uint64_t>(arg);
        }
      },
      val);
}

bool ValueHelper::toBool(const Value &val) {
  return std::visit(
      [](auto &&arg) -> bool {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
          return !arg.empty();
        } else if constexpr (std::is_same_v<T, bool>) {
          return arg;
        } else {
          return arg != 0;
        }
      },
      val);
}

std::string ValueHelper::toString(const Value &val) {
  return std::visit(
      [](auto &&arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
          return arg;
        } else if constexpr (std::is_same_v<T, bool>) {
          return arg ? "true" : "false";
        } else {
          return std::to_string(arg);
        }
      },
      val);
}

Value ValueHelper::add(const Value &a, const Value &b) {
  if (std::holds_alternative<std::string>(a) ||
      std::holds_alternative<std::string>(b)) {
    return toString(a) + toString(b);
  }

  DataType aType = getType(a);
  DataType bType = getType(b);

  // Unsigned operations
  if (aType == DataType::UINT8 || aType == DataType::UINT16 ||
      aType == DataType::UINT32 || aType == DataType::UINT64 ||
      bType == DataType::UINT8 || bType == DataType::UINT16 ||
      bType == DataType::UINT32 || bType == DataType::UINT64) {
    uint64_t result = toUInt64(a) + toUInt64(b);
    DataType resultType =
        (static_cast<int>(aType) > static_cast<int>(bType)) ? aType : bType;
    return createValue(resultType, result);
  }

  // Signed operations
  int64_t result = toInt64(a) + toInt64(b);
  DataType resultType =
      (static_cast<int>(aType) > static_cast<int>(bType)) ? aType : bType;
  return createValue(resultType, result);
}

Value ValueHelper::subtract(const Value &a, const Value &b) {
  DataType aType = getType(a);
  DataType bType = getType(b);

  if (aType == DataType::UINT8 || aType == DataType::UINT16 ||
      aType == DataType::UINT32 || aType == DataType::UINT64 ||
      bType == DataType::UINT8 || bType == DataType::UINT16 ||
      bType == DataType::UINT32 || bType == DataType::UINT64) {
    uint64_t result = toUInt64(a) - toUInt64(b);
    DataType resultType =
        (static_cast<int>(aType) > static_cast<int>(bType)) ? aType : bType;
    return createValue(resultType, result);
  }

  int64_t result = toInt64(a) - toInt64(b);
  DataType resultType =
      (static_cast<int>(aType) > static_cast<int>(bType)) ? aType : bType;
  return createValue(resultType, result);
}

Value ValueHelper::multiply(const Value &a, const Value &b) {
  DataType aType = getType(a);
  DataType bType = getType(b);

  if (aType == DataType::UINT8 || aType == DataType::UINT16 ||
      aType == DataType::UINT32 || aType == DataType::UINT64 ||
      bType == DataType::UINT8 || bType == DataType::UINT16 ||
      bType == DataType::UINT32 || bType == DataType::UINT64) {
    uint64_t result = toUInt64(a) * toUInt64(b);
    DataType resultType =
        (static_cast<int>(aType) > static_cast<int>(bType)) ? aType : bType;
    return createValue(resultType, result);
  }

  int64_t result = toInt64(a) * toInt64(b);
  DataType resultType =
      (static_cast<int>(aType) > static_cast<int>(bType)) ? aType : bType;
  return createValue(resultType, result);
}

Value ValueHelper::divide(const Value &a, const Value &b) {
  DataType aType = getType(a);
  DataType bType = getType(b);

  if (aType == DataType::UINT8 || aType == DataType::UINT16 ||
      aType == DataType::UINT32 || aType == DataType::UINT64 ||
      bType == DataType::UINT8 || bType == DataType::UINT16 ||
      bType == DataType::UINT32 || bType == DataType::UINT64) {
    uint64_t divisor = toUInt64(b);
    if (divisor == 0)
      throw std::runtime_error("Division by zero");
    uint64_t result = toUInt64(a) / divisor;
    DataType resultType =
        (static_cast<int>(aType) > static_cast<int>(bType)) ? aType : bType;
    return createValue(resultType, result);
  }

  int64_t divisor = toInt64(b);
  if (divisor == 0)
    throw std::runtime_error("Division by zero");
  int64_t result = toInt64(a) / divisor;
  DataType resultType =
      (static_cast<int>(aType) > static_cast<int>(bType)) ? aType : bType;
  return createValue(resultType, result);
}

Value ValueHelper::modulo(const Value &a, const Value &b) {
  DataType aType = getType(a);
  DataType bType = getType(b);

  if (aType == DataType::UINT8 || aType == DataType::UINT16 ||
      aType == DataType::UINT32 || aType == DataType::UINT64 ||
      bType == DataType::UINT8 || bType == DataType::UINT16 ||
      bType == DataType::UINT32 || bType == DataType::UINT64) {
    uint64_t divisor = toUInt64(b);
    if (divisor == 0)
      throw std::runtime_error("Modulo by zero");
    uint64_t result = toUInt64(a) % divisor;
    DataType resultType =
        (static_cast<int>(aType) > static_cast<int>(bType)) ? aType : bType;
    return createValue(resultType, result);
  }

  int64_t divisor = toInt64(b);
  if (divisor == 0)
    throw std::runtime_error("Modulo by zero");
  int64_t result = toInt64(a) % divisor;
  DataType resultType =
      (static_cast<int>(aType) > static_cast<int>(bType)) ? aType : bType;
  return createValue(resultType, result);
}

bool ValueHelper::greaterThan(const Value &a, const Value &b) {
  if (std::holds_alternative<std::string>(a) &&
      std::holds_alternative<std::string>(b)) {
    return std::get<std::string>(a) > std::get<std::string>(b);
  }
  return toInt64(a) > toInt64(b);
}

bool ValueHelper::lessThan(const Value &a, const Value &b) {
  if (std::holds_alternative<std::string>(a) &&
      std::holds_alternative<std::string>(b)) {
    return std::get<std::string>(a) < std::get<std::string>(b);
  }
  return toInt64(a) < toInt64(b);
}

bool ValueHelper::greaterOrEqual(const Value &a, const Value &b) {
  if (std::holds_alternative<std::string>(a) &&
      std::holds_alternative<std::string>(b)) {
    return std::get<std::string>(a) >= std::get<std::string>(b);
  }
  return toInt64(a) >= toInt64(b);
}

bool ValueHelper::lessOrEqual(const Value &a, const Value &b) {
  if (std::holds_alternative<std::string>(a) &&
      std::holds_alternative<std::string>(b)) {
    return std::get<std::string>(a) <= std::get<std::string>(b);
  }
  return toInt64(a) <= toInt64(b);
}

bool ValueHelper::equals(const Value &a, const Value &b) {
  if (a.index() != b.index())
    return false;
  return std::visit(
      [&b](auto &&arg) -> bool {
        using T = std::decay_t<decltype(arg)>;
        return arg == std::get<T>(b);
      },
      a);
}

bool ValueHelper::notEquals(const Value &a, const Value &b) {
  return !equals(a, b);
}

bool ValueHelper::logicalAnd(const Value &a, const Value &b) {
  return toBool(a) && toBool(b);
}

bool ValueHelper::logicalOr(const Value &a, const Value &b) {
  return toBool(a) || toBool(b);
}

bool ValueHelper::logicalNot(const Value &a) { return !toBool(a); }

Value ValueHelper::createValue(DataType type, int64_t intVal) {
  switch (type) {
  case DataType::INT8:
    return static_cast<int8_t>(intVal);
  case DataType::INT16:
    return static_cast<int16_t>(intVal);
  case DataType::INT32:
    return static_cast<int32_t>(intVal);
  case DataType::INT64:
    return intVal;
  case DataType::BOOL:
    return intVal != 0;
  default:
    return static_cast<int32_t>(intVal);
  }
}

Value ValueHelper::createValue(DataType type, uint64_t uintVal) {
  switch (type) {
  case DataType::UINT8:
    return static_cast<uint8_t>(uintVal);
  case DataType::UINT16:
    return static_cast<uint16_t>(uintVal);
  case DataType::UINT32:
    return static_cast<uint32_t>(uintVal);
  case DataType::UINT64:
    return uintVal;
  default:
    return static_cast<uint32_t>(uintVal);
  }
}

Value ValueHelper::createValue(DataType type, const std::string &strVal) {
  if (type == DataType::STRING)
    return strVal;
  throw std::runtime_error("Cannot create non-string value from string");
}

Value ValueHelper::createValue(DataType type, bool boolVal) {
  if (type == DataType::BOOL)
    return boolVal;
  throw std::runtime_error("Cannot create non-bool value from bool");
}

} // namespace Script
