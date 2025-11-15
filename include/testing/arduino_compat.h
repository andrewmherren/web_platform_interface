#ifndef ARDUINO_COMPAT_H
#define ARDUINO_COMPAT_H

// This file provides compatibility helpers for testing Arduino code in native
// environment

#include <ArduinoFake.h>

// Detect if we're in a native test environment
#if defined(ARDUINO)
#define IS_NATIVE_TEST 0
#else
#define IS_NATIVE_TEST 1
#endif

// Helper for setting up tests in both native and Arduino environments
#if IS_NATIVE_TEST
#define TEST_SETUP_BEGIN                                                       \
  int main(int argc, char **argv) {                                            \
    UNITY_BEGIN();
#define TEST_SETUP_END                                                         \
  UNITY_END();                                                                 \
  return 0;                                                                    \
  }
#define TEST_LOOP
#else
#define TEST_SETUP_BEGIN                                                       \
  void setup() {                                                               \
    UNITY_BEGIN();
#define TEST_SETUP_END                                                         \
  UNITY_END();                                                                 \
  }
#define TEST_LOOP                                                              \
  void loop() {}
#endif

#endif // ARDUINO_COMPAT_H