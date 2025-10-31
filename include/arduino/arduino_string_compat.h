#ifndef ARDUINO_STRING_COMPAT_H
#define ARDUINO_STRING_COMPAT_H

#ifdef NATIVE_PLATFORM

// We need to add the isEmpty() method to String for native testing
// This approach uses a template specialization to add the method
class String;

namespace ArduinoStringCompat {
    // Helper method to check if a String is empty in native environment
    inline bool isEmpty(const String& str) {
        return str.length() == 0;
    }
}

// Patch the String class with isEmpty() method
#define isEmpty() ArduinoStringCompat::isEmpty(*this)

#endif // NATIVE_PLATFORM

#endif // ARDUINO_STRING_COMPAT_H