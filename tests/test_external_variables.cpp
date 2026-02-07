#include "ScriptManager.h"
#include "DataTypes.h"
#include <gtest/gtest.h>

using namespace Script;

TEST(ExternalVariablesTest, ReadWriteVariable) {
  int32_t hostValue = 10;

  ScriptManager manager;
  manager.registerExternalVariable(
      "hostValue",
      [&]() -> Value { return static_cast<int32_t>(hostValue); },
      [&](const Value &v) { hostValue = std::get<int32_t>(v); });

  std::string source = "int32 bump() { hostValue = hostValue + 5; return hostValue; }";

  std::vector<CompilationError> errors;
  bool success = manager.loadScriptSource(source, "external_vars.script", errors);
  EXPECT_TRUE(success);
  EXPECT_TRUE(errors.empty());

  Value result;
  std::string errorMsg;
  success = manager.executeProcedure("bump", {}, result, errorMsg);
  EXPECT_TRUE(success);
  EXPECT_EQ(std::get<int32_t>(result), 15);
  EXPECT_EQ(hostValue, 15);
}

TEST(ExternalVariablesTest, ReadOnlyVariableRejectsWrite) {
  int32_t constantVal = 7;

  ScriptManager manager;
  manager.registerExternalVariableReadOnly(
      "constVal", [&]() -> Value { return static_cast<int32_t>(constantVal); });

  std::string source = "int32 test() { constVal = 2; return constVal; }";

  std::vector<CompilationError> errors;
  bool success = manager.loadScriptSource(source, "readonly.script", errors);
  EXPECT_TRUE(success);

  Value result;
  std::string errorMsg;
  success = manager.executeProcedure("test", {}, result, errorMsg);
  EXPECT_FALSE(success);
  EXPECT_NE(errorMsg.find("read-only"), std::string::npos);
  EXPECT_EQ(constantVal, 7);
}

TEST(ExternalVariablesTest, CompoundAssignUsesGetterAndSetter) {
  int32_t counter = 3;

  ScriptManager manager;
  manager.registerExternalVariable(
      "counter",
      [&]() -> Value { return static_cast<int32_t>(counter); },
      [&](const Value &v) { counter = std::get<int32_t>(v); });

  std::string source = "int32 update() { counter += 4; counter *= 2; return counter; }";

  std::vector<CompilationError> errors;
  bool success = manager.loadScriptSource(source, "compound.script", errors);
  EXPECT_TRUE(success);

  Value result;
  std::string errorMsg;
  success = manager.executeProcedure("update", {}, result, errorMsg);
  EXPECT_TRUE(success);
  EXPECT_EQ(std::get<int32_t>(result), 14);
  EXPECT_EQ(counter, 14);
}

TEST(ExternalVariablesTest, StringAndBoolRoundTrip) {
  std::string hostStr = "hi";
  bool hostFlag = false;

  ScriptManager manager;
  manager.registerExternalVariable(
      "hostStr",
      [&]() -> Value { return hostStr; },
      [&](const Value &v) { hostStr = std::get<std::string>(v); });

  manager.registerExternalVariable(
      "hostFlag",
      [&]() -> Value { return hostFlag; },
      [&](const Value &v) { hostFlag = std::get<bool>(v); });

  std::string source =
      "string touch() { hostStr = hostStr + \"!\"; hostFlag = !hostFlag; return hostFlag ? hostStr : \"\"; }";

  std::vector<CompilationError> errors;
  bool success = manager.loadScriptSource(source, "str_bool.script", errors);
  EXPECT_TRUE(success);

  Value result;
  std::string errorMsg;
  success = manager.executeProcedure("touch", {}, result, errorMsg);
  EXPECT_TRUE(success);
  EXPECT_EQ(std::get<std::string>(result), "hi!");
  EXPECT_EQ(hostStr, "hi!");
  EXPECT_TRUE(hostFlag);
}

TEST(ExternalVariablesTest, DoubleRoundTrip) {
  double host = 2.5;

  ScriptManager manager;
  manager.registerExternalVariable(
      "hostDouble",
      [&]() -> Value { return host; },
      [&](const Value &v) { host = std::get<double>(v); });

  std::string source =
      "double scale() { hostDouble = hostDouble * 1.5; return hostDouble; }";

  std::vector<CompilationError> errors;
  bool success = manager.loadScriptSource(source, "double.script", errors);
  EXPECT_TRUE(success);

  Value result;
  std::string errorMsg;
  success = manager.executeProcedure("scale", {}, result, errorMsg);
  EXPECT_TRUE(success);
  EXPECT_DOUBLE_EQ(std::get<double>(result), 3.75);
  EXPECT_DOUBLE_EQ(host, 3.75);
}

TEST(ExternalVariablesTest, ArrayRoundTripAndAssign) {
  Value hostVal = ValueHelper::createArray(
      TypeInfo(DataType::INT32),
      {static_cast<int32_t>(1), static_cast<int32_t>(2)});

  ScriptManager manager;
  manager.registerExternalVariable(
      "hostArr",
      [&]() -> Value { return hostVal; },
      [&](const Value &v) { hostVal = v; });

  std::string source =
      "int32 useArr() { push(hostArr, 5); hostArr[0] = hostArr[0] + 1; hostArr = [9, 10]; return len(hostArr); }";

  std::vector<CompilationError> errors;
  bool success = manager.loadScriptSource(source, "array.script", errors);
  EXPECT_TRUE(success);

  Value result;
  std::string errorMsg;
  success = manager.executeProcedure("useArr", {}, result, errorMsg);
  EXPECT_TRUE(success);
  EXPECT_EQ(std::get<int32_t>(result), 2);

  auto storedArr = ValueHelper::arrayElements(hostVal);
  ASSERT_EQ(storedArr.size(), 2u);
  EXPECT_EQ(std::get<int32_t>(storedArr[0]), 9);
  EXPECT_EQ(std::get<int32_t>(storedArr[1]), 10);
}
