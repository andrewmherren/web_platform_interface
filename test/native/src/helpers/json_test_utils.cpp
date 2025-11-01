#include "../../include/helpers/json_test_utils.h"
#include <ArduinoFake.h>
#include <ArduinoJson.h>
#include <unity.h>

void test_string_empty_check() {
  // Test string.length() == 0 pattern (standard C++)
  String emptyString = "";
  TEST_ASSERT_TRUE(emptyString.length() == 0);

  // Test non-empty string
  String nonEmptyString = "test";
  TEST_ASSERT_FALSE(nonEmptyString.length() == 0);
  TEST_ASSERT_TRUE(nonEmptyString.length() > 0);
}

void test_serialize_json_to_std_string() {
  // Test serializing JSON to std::string
  StaticJsonDocument<200> doc;
  doc["key"] = "value";
  doc["number"] = 42;

  // Test serialization using JsonTestUtils
  std::string result = JsonTestUtils::serializeToStdString(doc);

  TEST_ASSERT_TRUE(result.find("\"key\":\"value\"") != std::string::npos);
  TEST_ASSERT_TRUE(result.find("\"number\":42") != std::string::npos);
}

void test_deserialize_json_from_std_string() {
  // Test deserializing JSON from std::string
  std::string jsonStr = "{\"key\":\"value\",\"number\":42}";

  StaticJsonDocument<200> doc;
  DeserializationError error =
      JsonTestUtils::deserializeFromStdString(doc, jsonStr);

  TEST_ASSERT_TRUE(error == DeserializationError::Ok);
  TEST_ASSERT_EQUAL_STRING("value", doc["key"]);
  TEST_ASSERT_EQUAL(42, doc["number"]);
}

void register_json_test_utils_tests() {
  RUN_TEST(test_string_empty_check);
  RUN_TEST(test_serialize_json_to_std_string);
  RUN_TEST(test_deserialize_json_from_std_string);
}