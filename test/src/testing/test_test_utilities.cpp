#include "../../include/testing/test_test_utilities.h"
#include <ArduinoFake.h>
#include <ArduinoJson.h>
#include <interface/string_compat.h>
#include <testing/test_utilities.h> // Use the shared utilities
#include <unity.h>

// Test string compatibility helpers provided by the interface
void test_string_compatibility() {
  // Test that String operations work properly across platforms
  String testStr = "Hello, world!";
  TEST_ASSERT_EQUAL(13, testStr.length());
  TEST_ASSERT_TRUE(testStr.indexOf("Hello") == 0);

  // Test substring operations
  String subStr = testStr.substring(0, 5);
  TEST_ASSERT_EQUAL_STRING("Hello", subStr.c_str());

  // Test the utility function we provide
  TEST_ASSERT_TRUE(stringContains(testStr, "world"));
  TEST_ASSERT_FALSE(stringContains(testStr, "missing"));
}

// Test JSON serialization utilities provided by the interface
void test_json_serialization() {
  // Test the utility function we provide to modules
  String jsonStr = createTestJsonString();

  // Verify the string was created properly
  TEST_ASSERT_TRUE(jsonStr.length() > 0);
  TEST_ASSERT_TRUE(stringContains(jsonStr, "success"));
  TEST_ASSERT_TRUE(stringContains(jsonStr, "Test response"));
  TEST_ASSERT_TRUE(stringContains(jsonStr, "42"));

  // Test custom JSON creation
  String customJson = createTestJsonString("testKey", "testValue");
  TEST_ASSERT_TRUE(stringContains(customJson, "testKey"));
  TEST_ASSERT_TRUE(stringContains(customJson, "testValue"));

  // Test that we can parse the generated JSON back
  DynamicJsonDocument doc(256);
  DeserializationError error = deserializeJsonSafe(doc, jsonStr);
  TEST_ASSERT_TRUE(error == DeserializationError::Ok);
  TEST_ASSERT_TRUE(doc["success"].as<bool>());
  TEST_ASSERT_EQUAL(42, doc["count"].as<int>());
}

// Registration function to run all helper tests
void register_helper_tests() {
  RUN_TEST(test_string_compatibility);
  RUN_TEST(test_json_serialization);
}