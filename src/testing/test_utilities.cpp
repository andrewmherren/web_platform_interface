#include <testing/test_utilities.h>

// String compatibility helpers
bool stringContains(const String &str, const char *substring) {
  return str.indexOf(substring) >= 0;
}

// JSON testing helpers
String createTestJsonString() {
  DynamicJsonDocument doc(256);
  doc["success"] = true;
  doc["message"] = "Test response";
  doc["count"] = 42;
  return serializeJsonToString(doc);
}

String createTestJsonString(const char *key, const char *value) {
  DynamicJsonDocument doc(256);
  doc[key] = value;
  return serializeJsonToString(doc);
}

// Platform-specific serialization
String serializeJsonToString(const JsonDocument &doc) {
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

// JSON deserialization helper that handles platform differences
DeserializationError deserializeJsonSafe(JsonDocument &doc,
                                         const String &jsonStr) {
#ifdef NATIVE_PLATFORM
  // On native platform, use const char* to avoid Stream interface issues
  return deserializeJson(doc, jsonStr.c_str());
#else
  // On Arduino platform, String works fine directly
  return deserializeJson(doc, jsonStr);
#endif
}
