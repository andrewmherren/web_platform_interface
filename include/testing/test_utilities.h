#ifndef TEST_UTILITIES_H
#define TEST_UTILITIES_H

#include <Arduino.h>
#include <ArduinoJson.h>

#ifdef NATIVE_PLATFORM
#include <arduino/arduino_string_compat.h>
#endif

/**
 * Universal testing utilities that can be used by any WebPlatform module.
 * These provide cross-platform compatibility helpers for common testing needs.
 */

// String compatibility helpers
bool stringContains(const String &str, const char *substring);

// JSON testing helpers
String createTestJsonString();
String createTestJsonString(const char *key, const char *value);

// Platform-specific serialization helpers
String serializeJsonToString(const JsonDocument &doc);

// JSON deserialization helper that handles platform differences
DeserializationError deserializeJsonSafe(JsonDocument &doc,
                                         const String &jsonStr);

#endif // TEST_UTILITIES_H