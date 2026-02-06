# Real-World Application Example: E-Commerce Business Rules System

This example demonstrates a complete real-world application using the scripting system to separate business rules from application code.

## Overview

The example simulates an **e-commerce platform** where business rules for user registration, purchases, discounts, and reporting are defined in separate script files. This allows non-programmers (business analysts, product managers) to modify rules without touching C++ code.

## Architecture

### Script Modules (Business Rules)

1. **validation_rules.script** - Data validation logic
   - Age validation (18-120)
   - Username validation (3-20 characters)
   - Email format validation
   - Password strength validation

2. **business_logic.script** - Core business logic
   - User level calculation (Basic, Standard, Premium)
   - Discount calculation based on user level and purchase amount
   - Age-restricted product purchases (alcohol)
   - Price calculations

3. **reporting.script** - Formatting and reporting
   - User profile formatting
   - Purchase confirmation messages
   - Revenue reports

4. **workflows.script** - High-level workflows
   - User registration process
   - Purchase process with validation
   - User summary generation

### C++ Application Layer

- Loads all script modules
- Provides external functions (strlen, contains, logging, database)
- Orchestrates the workflow
- Handles UI and infrastructure

## Features Demonstrated

✅ **Multi-file script loading** - 4 separate script modules  
✅ **Cross-file procedure calls** - Scripts call each other's functions  
✅ **External C++ functions** - Scripts can call native functions  
✅ **All data types** - int8, int16, int32, uint8, uint16, uint32, bool, string  
✅ **String concatenation** - Automatic conversion and concatenation  
✅ **Complex logic** - Conditionals, calculations, validations  
✅ **Real-world scenarios** - User registration, purchases, discounts  

## Business Rules Examples

### User Level System
```script
// From business_logic.script
int32 calculateUserLevel(int32 age, int32 yearsActive) {
  if (age >= 60 || yearsActive >= 10) {
    return 3;  // Premium
  }
  if (age >= 30 || yearsActive >= 5) {
    return 2;  // Standard
  }
  return 1;  // Basic
}
```

### Dynamic Discount Calculation
```script
// From business_logic.script
int32 calculateDiscount(int32 userLevel, int32 purchaseAmount) {
  int32 baseDiscount = 0;
  
  if (userLevel == 3) {
    baseDiscount = 20;
  } else {
    if (userLevel == 2) {
      baseDiscount = 10;
    } else {
      baseDiscount = 5;
    }
  }
  
  // Bonus discount for large purchases
  if (purchaseAmount >= 1000) {
    baseDiscount = baseDiscount + 5;
  }
  
  return baseDiscount;
}
```

### Age-Restricted Products
```script
// From business_logic.script
bool canPurchaseAlcohol(int32 age, string country) {
  if (country == "USA") {
    return age >= 21;
  }
  if (country == "Japan") {
    return age >= 20;
  }
  return age >= 18;  // Default
}
```

## Test Scenarios

The test suite (`test_real_world_app.cpp`) covers:

1. **User Registration**
   - Valid registration
   - Underage users
   - Invalid username/email/password

2. **Purchase Processing**
   - Basic users with regular items
   - Premium users with large purchases
   - Age-restricted products (alcohol)
   - Discount calculations

3. **User Level System**
   - Level progression based on age and tenure
   - Discount tiers

4. **Reporting**
   - User summaries
   - Revenue reports

5. **Complete Lifecycle**
   - User registration → multiple purchases → level upgrades

## Running the Example

```bash
# Build
make test_real_world_app

# Run
./test_real_world_app

# Or run as part of test suite
ctest --output-on-failure
```

## Sample Output

```
═══════════════════════════════════════════════════
  Complete Application Workflow
═══════════════════════════════════════════════════

Step 1: User Registration
  Welcome, alice_smith! Your registration was successful.

Step 2: First Purchase (New User)
  Purchase confirmed for alice_smith. Total: $180
  Original: $200, Discount: 10%, Final: $180

Step 3: Purchase After 5 Years (Standard Level)
  Purchase confirmed for alice_smith. Total: $720
  Original: $800, Discount: 10%, Final: $720

Step 4: Large Purchase After 10 Years (Premium Level)
  Purchase confirmed for alice_smith. Total: $2250
  Original: $3000, Discount: 25%, Final: $2250

Step 5: User Summary After 10 Years
  User: alice_smith (Age: 45, Level: Premium)
  Average Discount: 25% | Total Spent: $15000
```

## Benefits of This Approach

1. **Separation of Concerns**
   - Business rules in scripts (easy to modify)
   - Application logic in C++ (performance critical)

2. **Non-Programmer Friendly**
   - Business analysts can modify rules
   - No compilation required for rule changes
   - Clear, readable syntax

3. **Type Safety**
   - Strong typing prevents errors
   - Validation at load time

4. **Performance**
   - Scripts parsed once, executed many times
   - No parsing overhead at runtime
   - AST interpretation is fast

5. **Maintainability**
   - Rules organized in logical modules
   - Easy to test individual rules
   - Version control friendly

## Extending the Example

You can easily add new rules by:

1. Creating new script files or adding to existing ones
2. Defining new procedures
3. Calling them from workflows or C++ code

No C++ recompilation needed for rule changes!

## Use Cases

This pattern works well for:
- Business rule engines
- Game logic/AI behavior
- Configuration-driven applications
- Dynamic pricing systems
- Workflow engines
- Policy management systems
