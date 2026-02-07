#include "ScriptManager.h"
#include <gtest/gtest.h>

using namespace Script;

TEST(BitwiseTest, BasicOpsAndShifts) {
  std::string source =
      "int32 ops(int32 a, int32 b) {"
      "  int32 r1 = a & b;"
      "  int32 r2 = a | b;"
      "  int32 r3 = a ^ b;"
      "  int32 r4 = a << 1;"
      "  int32 r5 = b >> 1;"
      "  return r1 + r2 + r3 + r4 + r5;"
      "}"
      "int32 notOp(int32 a) { return ~a; }";

  ScriptManager manager;
  std::vector<CompilationError> errors;
  ASSERT_TRUE(manager.loadScriptSource(source, "bitwise.script", errors));

  Value result;
  std::string errorMsg;

  ASSERT_TRUE(manager.executeProcedure("ops", {static_cast<int32_t>(6), static_cast<int32_t>(3)}, result, errorMsg)) << errorMsg;
  // a=6(110), b=3(011): r1=2, r2=7, r3=5, r4=12, r5=1 => sum=27
  EXPECT_EQ(std::get<int32_t>(result), 27);

  ASSERT_TRUE(manager.executeProcedure("notOp", {static_cast<int32_t>(0)}, result, errorMsg)) << errorMsg;
  EXPECT_EQ(std::get<int32_t>(result), -1);
}

TEST(BitwiseTest, UnsignedShifts) {
  std::string source =
      "uint32 mask(uint32 v) {"
      "  uint32 a = v << 2;"
      "  uint32 b = v >> 1;"
      "  return a | b;"
      "}";

  ScriptManager manager;
  std::vector<CompilationError> errors;
  ASSERT_TRUE(manager.loadScriptSource(source, "bitwise_u.script", errors));

  Value result;
  std::string errorMsg;

  ASSERT_TRUE(manager.executeProcedure("mask", {static_cast<uint32_t>(0b1010)}, result, errorMsg)) << errorMsg;
  EXPECT_EQ(std::get<uint32_t>(result), static_cast<uint32_t>((0b1010u << 2) | (0b1010u >> 1)));
}

TEST(BitwiseTest, RejectsNonIntegers) {
  std::string source =
      "int32 bad(string s) { return s & 1; }";

  ScriptManager manager;
  std::vector<CompilationError> errors;
  ASSERT_TRUE(manager.loadScriptSource(source, "bitwise_bad.script", errors));

  Value result;
  std::string errorMsg;
  bool ok = manager.executeProcedure("bad", {std::string("x")}, result, errorMsg);
  EXPECT_FALSE(ok);
  EXPECT_NE(errorMsg.find("only supports integers"), std::string::npos);
}
