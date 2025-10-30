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

// Forward declare the namespace for our custom converters
namespace ArduinoJson {
namespace ARDUINOJSON_VERSION_NAMESPACE {

// Custom converter for ArduinoFake String to work with ArduinoJson
// This addresses the missing 'write' method in ArduinoFake String
template<>
struct Converter<String> {
    static void toJson(const String& src, JsonVariant dst) {
        // Convert String to const char* for ArduinoJson
        dst.set(src.c_str());
    }
    
    static String fromJson(JsonVariantConst src) {
        // Convert from JSON back to String
        if (src.is<const char*>()) {
            return String(src.as<const char*>());
        }
        return String("");
    }
    
    static bool checkJson(JsonVariantConst src) {
        return src.is<const char*>();
    }
};

} // namespace ARDUINOJSON_VERSION_NAMESPACE
} // namespace ArduinoJson

// Extend ArduinoFake String with write method for ArduinoJson compatibility
// We can't modify ArduinoFake directly, but we can provide helper functions
namespace NativeJsonCompat {
    
    // Helper function to serialize JSON to String in native environment
    inline String serializeJsonToString(const JsonDocument& doc) {
        std::string stdStr;
        serializeJson(doc, stdStr);
        return String(stdStr.c_str());
    }
    
    // Helper function to deserialize JSON from String in native environment
    inline DeserializationError deserializeJsonFromString(JsonDocument& doc, const String& input) {
        return deserializeJson(doc, input.c_str());
    }
    
    // Helper to create JSON response with String values
    inline void setJsonString(JsonObject& obj, const char* key, const String& value) {
        obj[key] = value.c_str();
    }
    
    // Helper to get String from JSON
    inline String getJsonString(JsonObjectConst obj, const char* key, const String& defaultValue = String("")) {
        if (obj.containsKey(key) && obj[key].is<const char*>()) {
            return String(obj[key].as<const char*>());
        }
        return defaultValue;
    }
}

// Convenience macros for JSON operations in native tests
#define JSON_SET_STRING(obj, key, str) obj[key] = (str).c_str()
#define JSON_GET_STRING(obj, key) String((obj)[key].as<const char*>())

#endif // NATIVE_PLATFORM

#endif // ARDUINOJSON_COMPAT_H