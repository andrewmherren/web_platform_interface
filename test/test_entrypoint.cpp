#include <unity.h>

// Include native test headers for interface/type tests
// These are pure C++ tests with mocked Arduino APIs via ArduinoFake
#ifdef NATIVE_PLATFORM
#include <ArduinoFake.h>

// Include test headers - the corresponding .cpp files should be added to
// test_build_src in platformio.ini for proper compilation
#include "helpers/json_test_utils.h"
#include "interface/test_core_types.h"
#include "interface/utils/route_variant.h"
#include "interface/web_module_interface.h"
#include "interface/web_module_types.h"
#include "interface/web_platform_interface.h"
#include "interface/web_request.h"
#include "interface/web_response.h"
#include "testing/mock_web_platform.h"
#include "testing/mocks.h"
#include "testing/route_variant_native.h"
#include "testing/testing_platform_provider.h"
#include "testing/testing_platform_provider_json.h"

// Forward declarations for test registrars (defined in the included source
// files)
void register_core_types_tests();
void register_route_variant_tests();
void register_route_variant_native_tests();
void register_web_module_types_tests();
void register_web_response_tests();
void register_web_request_tests();
void register_mock_tests();
void register_json_test_utils_tests();
void register_web_module_interface_tests();
void register_web_platform_interface_tests();
void register_testing_platform_provider_json_tests();
void register_testing_platform_provider_tests();
void register_mock_web_platform_tests();

extern "C" void setUp(void) { ArduinoFakeReset(); }

extern "C" void tearDown(void) {
  // Clean teardown - nothing needed currently
}

int main(int argc, char **argv) {
  UNITY_BEGIN();

  // Register and run all interface/type test groups
  register_core_types_tests();
  register_route_variant_tests();
  register_route_variant_native_tests();
  register_web_module_types_tests();
  register_web_response_tests();
  register_web_request_tests();
  register_mock_tests();
  register_json_test_utils_tests();
  register_web_module_interface_tests();
  register_web_platform_interface_tests();
  register_testing_platform_provider_json_tests();
  register_testing_platform_provider_tests();
  register_mock_web_platform_tests();

  UNITY_END();
  return 0;
}

// ESP32 entrypoint - basic compilation test
#else
#include <Arduino.h>

// Minimal ESP32 test to verify the library compiles on hardware
// The interface library doesn't need extensive ESP32-specific tests since
// it's primarily type definitions and pure C++ interfaces

extern "C" void setUp(void) {}
extern "C" void tearDown(void) {}

void test_interface_library_compiles_on_esp32(void) {
  // Simple test to verify library compiles and links on ESP32
  TEST_ASSERT_TRUE(true);
}

void setup() {
  // Allow USB CDC/Serial to enumerate
  delay(2000);
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  UNITY_BEGIN();
  RUN_TEST(test_interface_library_compiles_on_esp32);
  UNITY_END();
}

void loop() {
  // No-op
}
#endif
