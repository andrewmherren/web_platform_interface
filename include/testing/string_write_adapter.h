#ifndef STRING_WRITE_ADAPTER_H
#define STRING_WRITE_ADAPTER_H

#ifdef NATIVE_PLATFORM

#include <Arduino.h>
#include <ArduinoFake.h>

// This file provides a write adapter for the ArduinoFake String class
// to make it compatible with ArduinoJson's serialization

// First, we need to extend the String class with write methods
namespace StringWriteAdapter {

// Adapter class that wraps String and provides write methods
class StringWriteWrapper {
private:
    String* str;

public:
    StringWriteWrapper(String& s) : str(&s) {}

    // Write a single byte - required by ArduinoJson
    size_t write(uint8_t c) {
        *str += (char)c;
        return 1;
    }

    // Write a buffer - required by ArduinoJson
    size_t write(const uint8_t* buffer, size_t size) {
        if (buffer == nullptr || size == 0)
            return 0;

        // Convert buffer to char* for concatenation
        char* charBuffer = new char[size + 1];
        memcpy(charBuffer, buffer, size);
        charBuffer[size] = '\0';
        
        *str += charBuffer;
        delete[] charBuffer;
        
        return size;
    }
};

// Function to create the wrapper
inline StringWriteWrapper getWriteAdapter(String& str) {
    return StringWriteWrapper(str);
}

// Helper functions for JSON serialization
inline String serializeJsonToString(const JsonDocument& doc) {
    String result;
    StringWriteWrapper adapter = getWriteAdapter(result);
    
    // Use the write adapter with ArduinoJson
    serializeJson(doc, adapter);
    return result;
}

} // namespace StringWriteAdapter

#endif // NATIVE_PLATFORM

#endif // STRING_WRITE_ADAPTER_H