#include "test_helpers.h"
#include <ArduinoFake.h>
#include <ArduinoJson.h>
#include <interface/string_compat.h>
#include <unity.h>


// Helper function for String comparison since Arduino Strings behave
// differently
bool stringContains(const String &str, const char *substring) {
  return str.indexOf(substring) >= 0;
}

// Helper function to create a simple JSON string for testing
String createTestJsonString() {
  DynamicJsonDocument doc(256);
  doc["success"] = true;
  doc["message"] = "Test response";
  doc["count"] = 42;

#ifdef NATIVE_PLATFORM
  std::string result;
  serializeJson(doc, result);
  return String(result.c_str());
#else
  String result;
  serializeJson(doc, result);
  return result;
#endif
}

void setUpCommon() {
  // Additional setup for mock infrastructure tests if needed
}

void tearDownCommon() {
  // Additional teardown for mock infrastructure tests if needed
}

// Test string compatibility helpers
void test_string_compatibility() {
  // Test that String operations work properly
  String testStr = "Hello, world!";
  TEST_ASSERT_EQUAL(13, testStr.length());
  TEST_ASSERT_TRUE(testStr.indexOf("Hello") == 0);

  // Test substring operations
  String subStr = testStr.substring(0, 5);
  TEST_ASSERT_EQUAL_STRING("Hello", subStr.c_str());
}

// Test JSON string serialization and deserialization
void test_json_serialization() {
  // Create a JSON document
  DynamicJsonDocument doc(256);
  doc["success"] = true;
  doc["message"] = "Test response";
  doc["count"] = 42;

#ifdef NATIVE_PLATFORM
  // Use std::string for native platform
  std::string jsonStr;
  serializeJson(doc, jsonStr);

  // Verify the string was created
  TEST_ASSERT_TRUE(jsonStr.length() > 0);
  TEST_ASSERT_TRUE(jsonStr.find("success") != std::string::npos);
  TEST_ASSERT_TRUE(jsonStr.find("Test response") != std::string::npos);

  // Parse back
  DynamicJsonDocument doc2(256);
  DeserializationError error = deserializeJson(doc2, jsonStr);
#else
  // Use String for Arduino platform
  String jsonStr;
  serializeJson(doc, jsonStr);

  // Verify the string was created
  TEST_ASSERT_TRUE(jsonStr.length() > 0);
  TEST_ASSERT_TRUE(stringContains(jsonStr, "success"));
  TEST_ASSERT_TRUE(stringContains(jsonStr, "Test response"));

  // Parse back
  DynamicJsonDocument doc2(256);
  DeserializationError error = deserializeJson(doc2, jsonStr);
#endif

  // Verify parsing succeeded
  TEST_ASSERT_TRUE(error == DeserializationError::Ok);
  TEST_ASSERT_TRUE(doc2["success"].as<bool>());
  TEST_ASSERT_EQUAL(42, doc2["count"].as<int>());
}

// Registration function to run all helper tests
void register_helper_tests() {
  RUN_TEST(test_string_compatibility);
  RUN_TEST(test_json_serialization);
}