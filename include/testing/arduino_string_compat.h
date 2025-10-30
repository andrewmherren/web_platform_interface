#ifndef ARDUINO_STRING_COMPAT_H
#define ARDUINO_STRING_COMPAT_H

#ifdef NATIVE_PLATFORM

#include <ArduinoFake.h>
#include <ArduinoJson.h>
#include <string>

// Arduino String compatibility for native testing
namespace ArduinoStringCompat {

// Helper method to check if a String is empty in native environment
inline bool isEmpty(const String &str) { return str.length() == 0; }

// Helper to convert to std::string for ArduinoJson compatibility
inline std::string toCStr(const String &str) {
  return std::string(str.c_str());
}

// Write adapter for ArduinoJson serialization with Arduino String
class StringWriter {
private:
  String *target;

public:
  StringWriter(String &s) : target(&s) {}

  size_t write(uint8_t c) {
    *target += (char)c;
    return 1;
  }

  size_t write(const uint8_t *buffer, size_t size) {
    if (!buffer || size == 0)
      return 0;
    for (size_t i = 0; i < size; i++) {
      *target += (char)buffer[i];
    }
    return size;
  }
};

// Read adapter for ArduinoJson deserialization with Arduino String
class StringReader {
private:
  const String *source;
  size_t pos;

public:
  StringReader(const String &s) : source(&s), pos(0) {}

  int read() {
    if (pos >= source->length()) {
      return -1;
    }
    return source->charAt(pos++);
  }
};

// Safe JSON serialization that works with ArduinoFake String
inline String serializeJsonSafe(const JsonDocument &doc) {
  String result;
  StringWriter writer(result);
  serializeJson(doc, writer);
  return result;
}

// Safe JSON deserialization that works with ArduinoFake String
inline DeserializationError deserializeJsonSafe(JsonDocument &doc,
                                                const String &input) {
  StringReader reader(input);
  return deserializeJson(doc, reader);
}

// Alternative using std::string (more reliable for ArduinoJson)
inline String serializeJsonViaStdString(const JsonDocument &doc) {
  std::string temp;
  serializeJson(doc, temp);
  return String(temp.c_str());
}

inline DeserializationError deserializeJsonViaStdString(JsonDocument &doc,
                                                        const String &input) {
  std::string temp = std::string(input.c_str());
  return deserializeJson(doc, temp);
}

} // namespace ArduinoStringCompat

// Macros for easy replacement in existing code
#define STRING_IS_EMPTY(str) ArduinoStringCompat::isEmpty(str)
#define SERIALIZE_JSON_SAFE(doc, str)                                          \
  str = ArduinoStringCompat::serializeJsonViaStdString(doc)
#define DESERIALIZE_JSON_SAFE(doc, str)                                        \
  ArduinoStringCompat::deserializeJsonViaStdString(doc, str)

#else

// For real Arduino, use normal methods
#define STRING_IS_EMPTY(str) (str).isEmpty()
#define SERIALIZE_JSON_SAFE(doc, str) serializeJson(doc, str)
#define DESERIALIZE_JSON_SAFE(doc, str) deserializeJson(doc, str)

#endif // NATIVE_PLATFORM

#endif // ARDUINO_STRING_COMPAT_H