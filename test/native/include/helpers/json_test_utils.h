#ifndef JSON_TEST_UTILS_H
#define JSON_TEST_UTILS_H

// JSON testing utilities for native tests
// These helpers make it easier to work with ArduinoJson in native test
// environment

#include <ArduinoJson.h>
#include <string>

namespace JsonTestUtils {

// Serialize JSON to std::string for native tests
inline std::string serializeToStdString(const JsonDocument &doc) {
  std::string result;
  serializeJson(doc, result);
  return result;
}

// Deserialize JSON from std::string for native tests
inline DeserializationError deserializeFromStdString(JsonDocument &doc,
                                                     const std::string &input) {
  return deserializeJson(doc, input);
}

} // namespace JsonTestUtils

// Test registration function
void register_json_test_utils_tests();

#endif // JSON_TEST_UTILS_H
