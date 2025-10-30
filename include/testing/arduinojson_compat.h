#ifndef ARDUINOJSON_COMPAT_H
#define ARDUINOJSON_COMPAT_H

// ArduinoJson compatibility layer for native testing
// This solves the fundamental incompatibility between ArduinoFake String
// and ArduinoJson's expectations, allowing module developers to test
// JSON functionality in native environments

#ifdef NATIVE_PLATFORM

#include <ArduinoFake.h>
#include <ArduinoJson.h>
#include <string>

// Simple, non-invasive approach to ArduinoJson compatibility
// Instead of modifying ArduinoJson internals, provide helper utilities
namespace NativeJsonCompat {

// Helper function to serialize JSON to String in native environment
inline String serializeJsonToString(const JsonDocument &doc) {
  std::string stdStr;
  serializeJson(doc, stdStr);
  return String(stdStr.c_str());
}

// Helper function to deserialize JSON from String in native environment
inline DeserializationError deserializeJsonFromString(JsonDocument &doc,
                                                      const String &input) {
  return deserializeJson(doc, input.c_str());
}

// Helper to create JSON response with String values
inline void setJsonString(JsonObject &obj, const char *key,
                          const String &value) {
  obj[key] = value.c_str();
}

// Helper to get String from JSON
inline String getJsonString(JsonObjectConst obj, const char *key,
                            const String &defaultValue = String("")) {
  if (obj.containsKey(key) && obj[key].is<const char *>()) {
    return String(obj[key].as<const char *>());
  }
  return defaultValue;
}

// Convenience macros for JSON operations in native tests
#define JSON_SET_STRING(obj, key, str) obj[key] = (str).c_str()
#define JSON_GET_STRING(obj, key) String((obj)[key].as<const char *>())

// Safe assignment helper that works in both Arduino and native
inline void assignStringToJson(JsonObject &obj, const char *key,
                               const String &value) {
  obj[key] = value.c_str();
}

} // namespace NativeJsonCompat

// Helper macros for cross-platform String assignment to JSON
#define JSON_ASSIGN_STRING(obj, key, str)                                      \
  NativeJsonCompat::assignStringToJson(obj, key, str)
#define JSON_SERIALIZE_TO_STRING(doc)                                          \
  NativeJsonCompat::serializeJsonToString(doc)

#else // Arduino environment

// In Arduino environment, regular assignment works
#define JSON_ASSIGN_STRING(obj, key, str) (obj)[key] = (str)
#define JSON_SERIALIZE_TO_STRING(doc)                                          \
  ([&]() {                                                                     \
    String result;                                                             \
    serializeJson(doc, result);                                                \
    return result;                                                             \
  }())

#endif // NATIVE_PLATFORM

#endif // ARDUINOJSON_COMPAT_H