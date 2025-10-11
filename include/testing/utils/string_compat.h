#ifndef STRING_COMPAT_H
#define STRING_COMPAT_H

// This file provides compatibility between Arduino String and std::string
// for native testing with ArduinoJson

#include <ArduinoFake.h>
#include <string>

// For native tests, convert between String and std::string
inline std::string toStdString(const String &arduinoString) {
  return std::string(arduinoString.c_str());
}

inline String toArduinoString(const std::string &stdString) {
  return String(stdString.c_str());
}

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