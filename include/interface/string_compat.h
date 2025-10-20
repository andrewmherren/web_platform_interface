#ifndef STRING_COMPAT_H
#define STRING_COMPAT_H

// This file provides compatibility between Arduino String and std::string
// for native testing with ArduinoJson, and unified API for interface files
// Works in both Arduino and native test environments

#ifdef NATIVE_PLATFORM
#include <ArduinoFake.h>
#else
#include <Arduino.h>
#endif
#include <string>

// ArduinoJson includes for JSON serialization functions
#ifdef NATIVE_PLATFORM
#include <ArduinoJson.h>
#else
#include <ArduinoJson.h>
#endif

// For native tests, convert between String and std::string
inline std::string toStdString(const String &arduinoString) {
  return std::string(arduinoString.c_str());
}

inline String toArduinoString(const std::string &stdString) {
  return String(stdString.c_str());
}

// String compatibility utilities - works in both Arduino and native
// environments
namespace StringUtils {

// Check if a String is empty - compatible with both Arduino and native
// environments Named differently to avoid macro conflicts
inline bool isStringEmpty(const String &str) {
#ifdef NATIVE_PLATFORM
  // ArduinoFake String doesn't have isEmpty()
  return str.length() == 0;
#else
  // Real Arduino String has isEmpty()
  return str.isEmpty();
#endif
}

} // namespace StringUtils

#ifdef NATIVE_PLATFORM
// Add missing String methods for native environment
inline bool isStringEmpty(const String &str) { return str.length() == 0; }

// Extension method for compatibility with Arduino String
namespace arduino_compat {
inline bool isStringEmpty(const String &str) { return str.length() == 0; }
} // namespace arduino_compat
#endif

// Use std::string for serialization in native tests
namespace StringCompat {
inline std::string serializeJsonToStdString(const JsonDocument &doc) {
  std::string result;
  serializeJson(doc, result);
  return result;
}

inline DeserializationError
deserializeJsonFromStdString(JsonDocument &doc, const std::string &input) {
  return deserializeJson(doc, input);
}
} // namespace StringCompat

#endif // STRING_COMPAT_H