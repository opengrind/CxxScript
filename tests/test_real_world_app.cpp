#include "ScriptManager.h"
#include <gtest/gtest.h>

using namespace Script;

// External C++ functions that the scripts can call
void registerExternalFunctions(ScriptManager &manager) {
  // String length function
  manager.registerExternalFunction(
      "strlen", [](const std::vector<Value> &args) -> Value {
        std::string str = std::get<std::string>(args[0]);
        return static_cast<int32_t>(str.length());
      });

  // String contains function
  manager.registerExternalFunction(
      "contains", [](const std::vector<Value> &args) -> Value {
        std::string str = std::get<std::string>(args[0]);
        std::string search = std::get<std::string>(args[1]);
        return str.find(search) != std::string::npos;
      });

  // Log function (simulates database logging)
  manager.registerExternalFunction(
      "log", [](const std::vector<Value> &args) -> Value {
        std::string message = std::get<std::string>(args[0]);
        std::cout << "      [LOG] " << message << std::endl;
        return true;
      });

  // Database save simulation
  manager.registerExternalFunction(
      "saveToDatabase", [](const std::vector<Value> &args) -> Value {
        std::string username = std::get<std::string>(args[0]);
        std::cout << "      [DB] Saved user: " << username << std::endl;
        return true;
      });
}

void printSection(const std::string &title) {
  std::cout << "\n" << std::string(60, '=') << std::endl;
  std::cout << "  " << title << std::endl;
  std::cout << std::string(60, '=') << std::endl;
}

TEST(RealWorldAppTest, UserRegistrationWorkflow) {
  printSection("User Registration System");

  ScriptManager manager;
  registerExternalFunctions(manager);

  std::vector<CompilationError> errors;

  // Load all script modules
  EXPECT_TRUE(manager.loadScriptFile("scripts/test_files/validation_rules.script", errors));
  std::cout << "    ✓ validation_rules.script loaded" << std::endl;

  EXPECT_TRUE(manager.loadScriptFile("scripts/test_files/business_logic.script", errors));
  std::cout << "    ✓ business_logic.script loaded" << std::endl;

  EXPECT_TRUE(manager.loadScriptFile("scripts/test_files/reporting.script", errors));
  std::cout << "    ✓ reporting.script loaded" << std::endl;

  EXPECT_TRUE(manager.loadScriptFile("scripts/test_files/workflows.script", errors));
  std::cout << "    ✓ workflows.script loaded" << std::endl;

  if (!errors.empty()) {
    for (const auto &err : errors) {
      std::cout << "    ERROR: " << err.toString() << std::endl;
    }
    EXPECT_TRUE(false);
  }


  // Test Case 1: Valid registration
  {
    std::vector<Value> args = {
        std::string("john_doe"), std::string("john@example.com"),
        std::string("secure123"), static_cast<int32_t>(25)};

    Value result;
    std::string errorMsg;
    bool success = manager.executeProcedure("processUserRegistration", args,
                                            result, errorMsg);
    EXPECT_TRUE(success);

    std::string message = std::get<std::string>(result);
    std::cout << "    Case 1: " << message << std::endl;
    EXPECT_TRUE(message.find("successful") != std::string::npos);
  }

  // Test Case 2: Underage user
  {
    std::vector<Value> args = {
        std::string("young_user"), std::string("young@example.com"),
        std::string("password123"), static_cast<int32_t>(16)};

    Value result;
    std::string errorMsg;
    bool success = manager.executeProcedure("processUserRegistration", args,
                                            result, errorMsg);
    EXPECT_TRUE(success);

    std::string message = std::get<std::string>(result);
    std::cout << "    Case 2: " << message << std::endl;
    EXPECT_TRUE(message.find("18 years old") != std::string::npos);
  }

  // Test Case 3: Invalid username
  {
    std::vector<Value> args = {std::string("ab"), // Too short
                               std::string("test@example.com"),
                               std::string("password123"),
                               static_cast<int32_t>(30)};

    Value result;
    std::string errorMsg;
    bool success = manager.executeProcedure("processUserRegistration", args,
                                            result, errorMsg);
    EXPECT_TRUE(success);

    std::string message = std::get<std::string>(result);
    std::cout << "    Case 3: " << message << std::endl;
    EXPECT_TRUE(message.find("Invalid username") != std::string::npos);
  }

  // Test Case 4: Invalid email
  {
    std::vector<Value> args = {std::string("testuser"),
                               std::string("notanemail"), // No @ or .
                               std::string("password123"),
                               static_cast<int32_t>(25)};

    Value result;
    std::string errorMsg;
    bool success = manager.executeProcedure("processUserRegistration", args,
                                            result, errorMsg);
    EXPECT_TRUE(success);

    std::string message = std::get<std::string>(result);
    std::cout << "    Case 4: " << message << std::endl;
    EXPECT_TRUE(message.find("Invalid email") != std::string::npos);
  }
}

TEST(RealWorldAppTest, PurchaseWorkflow) {
  printSection("E-Commerce Purchase System");

  ScriptManager manager;
  registerExternalFunctions(manager);

  std::vector<CompilationError> errors;

  // Load script modules
  manager.loadScriptFile("scripts/test_files/validation_rules.script", errors);
  manager.loadScriptFile("scripts/test_files/business_logic.script", errors);
  manager.loadScriptFile("scripts/test_files/reporting.script", errors);
  manager.loadScriptFile("scripts/test_files/workflows.script", errors);
  EXPECT_TRUE(errors.empty());


  // Test Case 1: Basic user, regular item
  {
    std::vector<Value> args = {std::string("alice"),
                               static_cast<int32_t>(25),  // age
                               static_cast<int32_t>(1),   // years active
                               static_cast<int32_t>(100), // item price
                               std::string("electronics")};

    Value result;
    std::string errorMsg;
    bool success =
        manager.executeProcedure("processPurchase", args, result, errorMsg);
    EXPECT_TRUE(success);

    std::string message = std::get<std::string>(result);
    std::cout << "    Case 1 (Basic user): " << message << std::endl;
    EXPECT_TRUE(message.find("confirmed") != std::string::npos);
    EXPECT_TRUE(message.find("$95") != std::string::npos); // 5% discount
  }

  // Test Case 2: Premium user, large purchase
  {
    std::vector<Value> args = {
        std::string("bob"),
        static_cast<int32_t>(65),   // age (premium)
        static_cast<int32_t>(15),   // years active
        static_cast<int32_t>(2000), // item price (gets bonus)
        std::string("furniture")};

    Value result;
    std::string errorMsg;
    bool success =
        manager.executeProcedure("processPurchase", args, result, errorMsg);
    EXPECT_TRUE(success);

    std::string message = std::get<std::string>(result);
    std::cout << "    Case 2 (Premium user, large purchase): " << message
              << std::endl;
    EXPECT_TRUE(message.find("confirmed") != std::string::npos);
    EXPECT_TRUE(message.find("25%") != std::string::npos);   // 20% + 5% bonus
    EXPECT_TRUE(message.find("$1500") != std::string::npos); // 25% off 2000
  }

  // Test Case 3: Underage attempting alcohol purchase
  {
    std::vector<Value> args = {std::string("charlie"),
                               static_cast<int32_t>(19), // age
                               static_cast<int32_t>(2),  // years active
                               static_cast<int32_t>(50), // item price
                               std::string("alcohol")};

    Value result;
    std::string errorMsg;
    bool success =
        manager.executeProcedure("processPurchase", args, result, errorMsg);
    EXPECT_TRUE(success);

    std::string message = std::get<std::string>(result);
    std::cout << "    Case 3 (Underage alcohol): " << message << std::endl;
    EXPECT_TRUE(message.find("denied") != std::string::npos);
  }

  // Test Case 4: Legal age alcohol purchase
  {
    std::vector<Value> args = {std::string("dave"),
                               static_cast<int32_t>(25), // age
                               static_cast<int32_t>(3),  // years active
                               static_cast<int32_t>(75), // item price
                               std::string("alcohol")};

    Value result;
    std::string errorMsg;
    bool success =
        manager.executeProcedure("processPurchase", args, result, errorMsg);
    EXPECT_TRUE(success);

    std::string message = std::get<std::string>(result);
    std::cout << "    Case 4 (Legal alcohol purchase): " << message
              << std::endl;
    EXPECT_TRUE(message.find("confirmed") != std::string::npos);
  }
}

TEST(RealWorldAppTest, UserLevelSystem) {
  printSection("User Level & Discount Calculation");

  ScriptManager manager;
  registerExternalFunctions(manager);

  std::vector<CompilationError> errors;
  manager.loadScriptFile("scripts/test_files/business_logic.script", errors);
  EXPECT_TRUE(errors.empty());


  struct TestCase {
    int32_t age;
    int32_t years;
    int32_t expectedLevel;
    std::string description;
  };

  TestCase cases[] = {{25, 1, 1, "Young new user -> Basic"},
                      {35, 2, 2, "Mid-age user -> Standard"},
                      {25, 10, 3, "Long-term user -> Premium"},
                      {65, 1, 3, "Senior user -> Premium"},
                      {45, 7, 2, "Mid-age, medium tenure -> Standard"}};

  for (const auto &testCase : cases) {
    std::vector<Value> args = {testCase.age, testCase.years};
    Value result;
    std::string errorMsg;

    bool success =
        manager.executeProcedure("calculateUserLevel", args, result, errorMsg);
    EXPECT_TRUE(success);

    int32_t level = std::get<int32_t>(result);
    std::cout << "    " << testCase.description << " (Age: " << testCase.age
              << ", Years: " << testCase.years << ") -> Level " << level
              << std::endl;
    EXPECT_TRUE(level == testCase.expectedLevel);
  }


  // Level 1, small purchase
  {
    std::vector<Value> args = {static_cast<int32_t>(1),
                               static_cast<int32_t>(500)};
    Value result;
    std::string errorMsg;
    bool success =
        manager.executeProcedure("calculateDiscount", args, result, errorMsg);
    EXPECT_TRUE(success);
    EXPECT_TRUE(std::get<int32_t>(result) == 5);
    std::cout << "    Level 1, $500 purchase -> 5% discount" << std::endl;
  }

  // Level 3, large purchase
  {
    std::vector<Value> args = {static_cast<int32_t>(3),
                               static_cast<int32_t>(1500)};
    Value result;
    std::string errorMsg;
    bool success =
        manager.executeProcedure("calculateDiscount", args, result, errorMsg);
    EXPECT_TRUE(success);
    EXPECT_TRUE(std::get<int32_t>(result) == 25); // 20% + 5% bonus
    std::cout << "    Level 3, $1500 purchase -> 25% discount (20% + 5% bonus)"
              << std::endl;
  }
}

TEST(RealWorldAppTest, ReportingSystem) {
  printSection("Reporting & Formatting System");

  ScriptManager manager;
  registerExternalFunctions(manager);

  std::vector<CompilationError> errors;
  manager.loadScriptFile("scripts/test_files/business_logic.script", errors);
  manager.loadScriptFile("scripts/test_files/reporting.script", errors);
  manager.loadScriptFile("scripts/test_files/workflows.script", errors);
  EXPECT_TRUE(errors.empty());


  // Generate comprehensive user summary
  {
    std::vector<Value> args = {
        std::string("premium_user"), static_cast<int32_t>(62),
        static_cast<int32_t>(8), static_cast<int32_t>(5000)};

    Value result;
    std::string errorMsg;
    bool success =
        manager.executeProcedure("getUserSummary", args, result, errorMsg);
    EXPECT_TRUE(success);

    std::string summary = std::get<std::string>(result);
    std::cout << "    " << summary << std::endl;
    EXPECT_TRUE(summary.find("premium_user") != std::string::npos);
    EXPECT_TRUE(summary.find("Premium") != std::string::npos);
  }


  // Calculate and format revenue
  {
    std::vector<Value> args = {static_cast<int32_t>(100),
                               static_cast<int32_t>(50)};
    Value result;
    std::string errorMsg;
    bool success = manager.executeProcedure("calculateTotalRevenue", args,
                                            result, errorMsg);
    EXPECT_TRUE(success);
    int32_t revenue = std::get<int32_t>(result);
    EXPECT_TRUE(revenue == 5000);

    // Format the report
    std::vector<Value> reportArgs = {static_cast<int32_t>(100),
                                     static_cast<int32_t>(50), revenue};
    success = manager.executeProcedure("formatRevenueReport", reportArgs,
                                       result, errorMsg);
    EXPECT_TRUE(success);

    std::string report = std::get<std::string>(result);
    std::cout << "    " << report << std::endl;
    EXPECT_TRUE(report.find("50 items") != std::string::npos);
    EXPECT_TRUE(report.find("$5000") != std::string::npos);
  }
}

TEST(RealWorldAppTest, CompleteWorkflow) {
  printSection("Complete Application Workflow");

  ScriptManager manager;
  registerExternalFunctions(manager);

  std::vector<CompilationError> errors;
  manager.loadScriptFile("scripts/test_files/validation_rules.script", errors);
  manager.loadScriptFile("scripts/test_files/business_logic.script", errors);
  manager.loadScriptFile("scripts/test_files/reporting.script", errors);
  manager.loadScriptFile("scripts/test_files/workflows.script", errors);

  if (!errors.empty()) {
    for (const auto &err : errors) {
      std::cout << "ERROR: " << err.toString() << std::endl;
    }
    EXPECT_TRUE(false);
  }


  // Step 1: Register user
  std::cout << "\n  Step 1: User Registration" << std::endl;
  {
    std::vector<Value> args = {
        std::string("alice_smith"), std::string("alice@company.com"),
        std::string("securePass2024"), static_cast<int32_t>(35)};

    Value result;
    std::string errorMsg;
    bool success = manager.executeProcedure("processUserRegistration", args,
                                            result, errorMsg);
    EXPECT_TRUE(success);
    std::cout << "    " << std::get<std::string>(result) << std::endl;
  }

  // Step 2: First purchase (new user)
  std::cout << "\n  Step 2: First Purchase (New User)" << std::endl;
  {
    std::vector<Value> args = {
        std::string("alice_smith"), static_cast<int32_t>(35),
        static_cast<int32_t>(0), static_cast<int32_t>(200),
        std::string("electronics")};

    Value result;
    std::string errorMsg;
    bool success =
        manager.executeProcedure("processPurchase", args, result, errorMsg);
    EXPECT_TRUE(success);
    std::cout << "    " << std::get<std::string>(result) << std::endl;
  }

  // Step 3: Purchase after 5 years (upgraded to Standard)
  std::cout << "\n  Step 3: Purchase After 5 Years (Standard Level)"
            << std::endl;
  {
    std::vector<Value> args = {
        std::string("alice_smith"), static_cast<int32_t>(40),
        static_cast<int32_t>(5), static_cast<int32_t>(800),
        std::string("furniture")};

    Value result;
    std::string errorMsg;
    bool success =
        manager.executeProcedure("processPurchase", args, result, errorMsg);
    EXPECT_TRUE(success);
    std::cout << "    " << std::get<std::string>(result) << std::endl;
  }

  // Step 4: Large purchase after 10 years (Premium)
  std::cout << "\n  Step 4: Large Purchase After 10 Years (Premium Level)"
            << std::endl;
  {
    std::vector<Value> args = {
        std::string("alice_smith"), static_cast<int32_t>(45),
        static_cast<int32_t>(10), static_cast<int32_t>(3000),
        std::string("appliances")};

    Value result;
    std::string errorMsg;
    bool success =
        manager.executeProcedure("processPurchase", args, result, errorMsg);
    EXPECT_TRUE(success);
    std::cout << "    " << std::get<std::string>(result) << std::endl;
  }

  // Step 5: Generate final user summary
  std::cout << "\n  Step 5: User Summary After 10 Years" << std::endl;
  {
    std::vector<Value> args = {
        std::string("alice_smith"), static_cast<int32_t>(45),
        static_cast<int32_t>(10), static_cast<int32_t>(15000)};

    Value result;
    std::string errorMsg;
    bool success =
        manager.executeProcedure("getUserSummary", args, result, errorMsg);
    EXPECT_TRUE(success);
    std::cout << "    " << std::get<std::string>(result) << std::endl;
  }
}

