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

// String write adapter for ArduinoJson compatibility
class StringWriteAdapter {
private:
  String *str;

public:
  StringWriteAdapter(String &s) : str(&s) {}

  // Write a single byte - required by ArduinoJson
  size_t write(uint8_t c) {
    *str += (char)c;
    return 1;
  }

  // Write a buffer - required by ArduinoJson
  size_t write(const uint8_t *buffer, size_t size) {
    if (buffer == nullptr || size == 0)
      return 0;

    // Convert buffer to string and append
    for (size_t i = 0; i < size; i++) {
      *str += (char)buffer[i];
    }
    return size;
  }
};

// String read adapter for ArduinoJson compatibility
class StringReadAdapter {
private:
  const String *str;
  size_t position;

public:
  StringReadAdapter(const String &s) : str(&s), position(0) {}

  // Read a single byte - required by ArduinoJson
  int read() {
    if (position >= str->length()) {
      return -1; // End of string
    }
    return str->charAt(position++);
  }
};

// Extension method for compatibility with Arduino String
namespace arduino_compat {
inline bool isStringEmpty(const String &str) { return str.length() == 0; }

// Helper functions for JSON serialization with String compatibility
inline String serializeJsonToArduinoString(const JsonDocument &doc) {
  String result;
  StringWriteAdapter adapter(result);
  serializeJson(doc, adapter);
  return result;
}

inline DeserializationError
deserializeJsonFromArduinoString(JsonDocument &doc, const String &input) {
  StringReadAdapter adapter(input);
  return deserializeJson(doc, adapter);
}

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

// ESP32 compatibility moved to a separate header to avoid conflicts
#ifdef NATIVE_PLATFORM
#include <testing/esp32_compat.h>
#endif

#endif // STRING_COMPAT_H