#include "../../include/interface/test_string_compat.h"
#include <ArduinoFake.h>
#include <ArduinoJson.h>
#include <interface/string_compat.h>
#include <unity.h>

void test_string_empty_check() {
  // Test empty string with global isStringEmpty function (line 51)
  String emptyString = "";
  TEST_ASSERT_TRUE(isStringEmpty(emptyString));

  // Test non-empty string with global isStringEmpty function
  String nonEmptyString = "test";
  TEST_ASSERT_FALSE(isStringEmpty(nonEmptyString));

  // Test StringUtils namespace version
  TEST_ASSERT_TRUE(StringUtils::isStringEmpty(emptyString));
  TEST_ASSERT_FALSE(StringUtils::isStringEmpty(nonEmptyString));

  // Test arduino_compat namespace version
  TEST_ASSERT_TRUE(arduino_compat::isStringEmpty(emptyString));
  TEST_ASSERT_FALSE(arduino_compat::isStringEmpty(nonEmptyString));
}

void test_serialize_json_to_std_string() {
  // Test serializing JSON to std::string
  StaticJsonDocument<200> doc;
  doc["key"] = "value";
  doc["number"] = 42;

  // Test serialization using the StringCompat namespace
  std::string result = StringCompat::serializeJsonToStdString(doc);

  TEST_ASSERT_TRUE(result.find("\"key\":\"value\"") != std::string::npos);
  TEST_ASSERT_TRUE(result.find("\"number\":42") != std::string::npos);
}

void test_deserialize_json_from_std_string() {
  // Test deserializing JSON from std::string
  std::string jsonStr = "{\"key\":\"value\",\"number\":42}";

  StaticJsonDocument<200> doc;
  DeserializationError error =
      StringCompat::deserializeJsonFromStdString(doc, jsonStr);

  TEST_ASSERT_TRUE(error == DeserializationError::Ok);
  TEST_ASSERT_EQUAL_STRING("value", doc["key"]);
  TEST_ASSERT_EQUAL(42, doc["number"]);
}

void register_string_compat_tests() {
  RUN_TEST(test_string_empty_check);
  RUN_TEST(test_serialize_json_to_std_string);
  RUN_TEST(test_deserialize_json_from_std_string);
}