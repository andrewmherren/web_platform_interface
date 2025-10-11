#ifndef INTERFACE_STRING_COMPAT_H
#define INTERFACE_STRING_COMPAT_H

#include <Arduino.h>

// String compatibility utilities for interface files
// Provides unified API that works in both Arduino and native test environments

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

// Additional utilities can be added here as needed

} // namespace StringUtils

#endif // INTERFACE_STRING_COMPAT_H