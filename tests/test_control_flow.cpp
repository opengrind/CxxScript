#include "ScriptManager.h"
#include <gtest/gtest.h>

using namespace Script;

TEST(ControlFlowTest, BreakAndContinueInWhile) {
  std::string source =
      "int32 loop() {"
      "  int32 i = 0;"
      "  int32 sum = 0;"
      "  while (i < 10) {"
      "    i += 1;"
      "    if (i == 3) { continue; }"
      "    if (i == 7) { break; }"
      "    sum += i;"
      "  }"
      "  return sum;"
      "}";

  ScriptManager manager;
  std::vector<CompilationError> errors;
  ASSERT_TRUE(manager.loadScriptSource(source, "loop.script", errors));

  Value result;
  std::string errorMsg;
  ASSERT_TRUE(manager.executeProcedure("loop", {}, result, errorMsg)) << errorMsg;
  // i sequence added: 1,2,4,5,6 => sum = 18
  EXPECT_EQ(std::get<int32_t>(result), 18);
}

TEST(ControlFlowTest, ContinueInForRunsIncrement) {
  std::string source =
      "int32 loop() {"
      "  int32 i = 0;"
      "  int32 hits = 0;"
      "  for (i = 0; i < 5; i += 1) {"
      "    if (i % 2 == 0) { continue; }"
      "    hits += i;"
      "  }"
      "  return hits;"
      "}";

  ScriptManager manager;
  std::vector<CompilationError> errors;
  ASSERT_TRUE(manager.loadScriptSource(source, "for.script", errors));

  Value result;
  std::string errorMsg;
  ASSERT_TRUE(manager.executeProcedure("loop", {}, result, errorMsg)) << errorMsg;
  // Odd indices: 1 + 3 = 4
  EXPECT_EQ(std::get<int32_t>(result), 4);
}

TEST(ControlFlowTest, DoWhileExecutesAtLeastOnce) {
  std::string source =
      "int32 test() {"
      "  int32 i = 0;"
      "  do { i += 1; } while (i < 0);"
      "  return i;"
      "}";

  ScriptManager manager;
  std::vector<CompilationError> errors;
  ASSERT_TRUE(manager.loadScriptSource(source, "dowhile.script", errors));

  Value result;
  std::string errorMsg;
  ASSERT_TRUE(manager.executeProcedure("test", {}, result, errorMsg)) << errorMsg;
  EXPECT_EQ(std::get<int32_t>(result), 1);
}

TEST(ControlFlowTest, SwitchMatchesCaseAndDefault) {
  std::string source =
      "int32 choose(int32 v) {"
      "  int32 out = 0;"
      "  switch (v) {"
      "    case 1: out = 10; break;"
      "    case 2: out = 20; break;"
      "    default: out = 99; break;"
      "  }"
      "  return out;"
      "}";

  ScriptManager manager;
  std::vector<CompilationError> errors;
  ASSERT_TRUE(manager.loadScriptSource(source, "switch.script", errors));

  Value result;
  std::string errorMsg;

  ASSERT_TRUE(manager.executeProcedure("choose", {static_cast<int32_t>(1)}, result, errorMsg));
  EXPECT_EQ(std::get<int32_t>(result), 10);

  ASSERT_TRUE(manager.executeProcedure("choose", {static_cast<int32_t>(2)}, result, errorMsg));
  EXPECT_EQ(std::get<int32_t>(result), 20);

  ASSERT_TRUE(manager.executeProcedure("choose", {static_cast<int32_t>(5)}, result, errorMsg));
  EXPECT_EQ(std::get<int32_t>(result), 99);
}

TEST(ControlFlowTest, SwitchFallthroughUntilBreak) {
  std::string source =
      "int32 test(int32 v) {"
      "  int32 out = 0;"
      "  switch (v) {"
      "    case 1: out += 1;"
      "    case 2: out += 2;"
      "    case 3: out += 3; break;"
      "    default: out = -1;"
      "  }"
      "  return out;"
      "}";

  ScriptManager manager;
  std::vector<CompilationError> errors;
  ASSERT_TRUE(manager.loadScriptSource(source, "switch_fall.script", errors));

  Value result;
  std::string errorMsg;

  ASSERT_TRUE(manager.executeProcedure("test", {static_cast<int32_t>(1)}, result, errorMsg));
  EXPECT_EQ(std::get<int32_t>(result), 6); // 1+2+3

  ASSERT_TRUE(manager.executeProcedure("test", {static_cast<int32_t>(2)}, result, errorMsg));
  EXPECT_EQ(std::get<int32_t>(result), 5); // 2+3

  ASSERT_TRUE(manager.executeProcedure("test", {static_cast<int32_t>(3)}, result, errorMsg));
  EXPECT_EQ(std::get<int32_t>(result), 3); // 3
}

TEST(ControlFlowTest, TernaryExpression) {
  std::string source =
      "int32 pick(int32 a, int32 b) {"
      "  int32 max = (a > b) ? a : b;"
      "  int32 min = (a < b) ? a : b;"
      "  return max - min;"
      "}"
      "int32 nested(int32 x) {"
      "  return (x > 0) ? (x > 5 ? 2 : 1) : 0;"
      "}";

  ScriptManager manager;
  std::vector<CompilationError> errors;
  ASSERT_TRUE(manager.loadScriptSource(source, "ternary.script", errors));

  Value result;
  std::string errorMsg;

  ASSERT_TRUE(manager.executeProcedure("pick", {static_cast<int32_t>(3), static_cast<int32_t>(7)}, result, errorMsg));
  EXPECT_EQ(std::get<int32_t>(result), 4);

  ASSERT_TRUE(manager.executeProcedure("nested", {static_cast<int32_t>(2)}, result, errorMsg));
  EXPECT_EQ(std::get<int32_t>(result), 1);

  ASSERT_TRUE(manager.executeProcedure("nested", {static_cast<int32_t>(6)}, result, errorMsg));
  EXPECT_EQ(std::get<int32_t>(result), 2);

  ASSERT_TRUE(manager.executeProcedure("nested", {static_cast<int32_t>(-1)}, result, errorMsg));
  EXPECT_EQ(std::get<int32_t>(result), 0);
}
