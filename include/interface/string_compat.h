// Cross-platform String compatibility layer
#ifndef WEB_PLATFORM_INTERFACE_STRING_COMPAT_H
#define WEB_PLATFORM_INTERFACE_STRING_COMPAT_H

#if defined(ARDUINO) || defined(ESP_PLATFORM)
// Real Arduino environment
#include <Arduino.h>
#elif defined(NATIVE_PLATFORM)
// Native test environment with ArduinoFake
// ArduinoFake provides its own String class, so just include it
#include <Arduino.h>
#else
// Pure native build without Arduino at all
#include <string>
using String = std::string;
#endif

#endif // WEB_PLATFORM_INTERFACE_STRING_COMPAT_H
