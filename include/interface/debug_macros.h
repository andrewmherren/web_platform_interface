#ifndef WEB_PLATFORM_INTERFACE_DEBUG_MACROS_H
#define WEB_PLATFORM_INTERFACE_DEBUG_MACROS_H

/**
 * Unified Debug Macros for WebPlatform Interface
 *
 * Provides compile-time switchable debug output to reduce heap usage and
 * fragmentation on ESP32/ESP8266 devices. When DEBUG_ENABLED is 0, all debug
 * calls are completely removed from compiled code, consuming zero resources.
 *
 * This file defines the canonical debug macros used by both
 * web_platform_interface and web_platform libraries.
 */

#ifdef NATIVE_PLATFORM
#include <ArduinoFake.h>
#include <cstdio>
#include <iostream>

#else
#include <Arduino.h>
#endif

// Debug can be enabled via PlatformIO build flag: -DWEB_PLATFORM_DEBUG
// Or by defining WEB_PLATFORM_DEBUG before including this header
#ifndef WEB_PLATFORM_DEBUG
#define WEB_PLATFORM_DEBUG 0 // Default to disabled for production builds
#endif

#define DEBUG_ENABLED WEB_PLATFORM_DEBUG

#if DEBUG_ENABLED
#ifdef NATIVE_PLATFORM
// Native testing debug output
#define DEBUG_BEGIN(baud) // No-op for native
#define DEBUG_PRINT(x) std::cout << x
#define DEBUG_PRINTLN(x) std::cout << x << std::endl
#define DEBUG_PRINTF(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
// Arduino/ESP32 debug output
#define DEBUG_BEGIN(baud) Serial.begin(baud)
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTLN(x) Serial.println(x)
#define DEBUG_PRINTF(fmt, ...) Serial.printf(fmt, ##__VA_ARGS__)
#endif
#else
// When DEBUG_ENABLED is 0, all debug macros compile to nothing
#define DEBUG_BEGIN(baud)
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINTF(fmt, ...)
#endif

/**
 * Always-enabled critical error reporting (not affected by DEBUG_ENABLED)
 * Use these for errors that should always be reported regardless of debug mode
 */
#ifdef NATIVE_PLATFORM
// Native testing - use std::cout and printf
#define ERROR_PRINT(x) std::cout << "[ERROR] " << x
#define ERROR_PRINTLN(x) std::cout << "[ERROR] " << x << std::endl
#define ERROR_PRINTF(fmt, ...) printf("[ERROR] " fmt, ##__VA_ARGS__)
#else
// Arduino/ESP32 - use Serial with printf
#define ERROR_PRINT(x)                                                         \
  Serial.print("[ERROR] ");                                                    \
  Serial.print(x)
#define ERROR_PRINTLN(x)                                                       \
  Serial.print("[ERROR] ");                                                    \
  Serial.println(x)
#define ERROR_PRINTF(fmt, ...) Serial.printf("[ERROR] " fmt, ##__VA_ARGS__)
#endif

/**
 * Always-enabled warning reporting (not affected by DEBUG_ENABLED)
 * Use these for warnings that should always be reported regardless of debug
 * mode
 */
#ifdef NATIVE_PLATFORM
// Native testing - use std::cout and printf
#define WARN_PRINT(x) std::cout << "[WARN] " << x
#define WARN_PRINTLN(x) std::cout << "[WARN] " << x << std::endl
#define WARN_PRINTF(fmt, ...) printf("[WARN] " fmt, ##__VA_ARGS__)
#else
// Arduino/ESP32 - use Serial with printf
#define WARN_PRINT(x)                                                          \
  Serial.print("[WARN] ");                                                     \
  Serial.print(x)
#define WARN_PRINTLN(x)                                                        \
  Serial.print("[WARN] ");                                                     \
  Serial.println(x)
#define WARN_PRINTF(fmt, ...) Serial.printf("[WARN] " fmt, ##__VA_ARGS__)
#endif

#endif // WEB_PLATFORM_INTERFACE_DEBUG_MACROS_H