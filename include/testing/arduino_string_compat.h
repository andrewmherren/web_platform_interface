#ifndef ARDUINO_STRING_COMPAT_H
#define ARDUINO_STRING_COMPAT_H

#ifdef NATIVE_PLATFORM

// Arduino String compatibility for native testing
// Instead of macros, provide utility functions
class String;

namespace ArduinoStringCompat {
// Helper method to check if a String is empty in native environment
inline bool isEmpty(const String &str) { return str.length() == 0; }

// Helper to convert to std::string for ArduinoJson compatibility
inline std::string toCStr(const String &str) {
  return std::string(str.c_str());
}
} // namespace ArduinoStringCompat

// For code that uses str.isEmpty(), provide a function-based alternative
// Don't use macros as they cause parsing issues
#define STRING_IS_EMPTY(str) ArduinoStringCompat::isEmpty(str)

#endif // NATIVE_PLATFORM

#endif // ARDUINO_STRING_COMPAT_H