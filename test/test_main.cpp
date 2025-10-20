#include <ArduinoFake.h>
#include <unity.h>

// Include all test header files
#include "test_core_types.h"
#include "test_helpers.h"
#include "test_mocks.h"
#include "test_route_variant.h"
#include "test_testing_platform_provider_json.h"
#include "test_web_module_interface.h"
#include "test_web_module_types.h"
#include "test_web_platform_interface.h"
#include "test_web_request.h"
#include "test_web_response.h"

void setUp() {
  ArduinoFakeReset();
  setUpCommon();
}

void tearDown() { tearDownCommon(); }

#ifdef NATIVE_PLATFORM
int main(int argc, char **argv) {
  UNITY_BEGIN();

  // Register and run all test groups
  register_core_types_tests();
  register_route_variant_tests();
  register_web_module_types_tests();
  // Re-enable web_response tests with selectively disabled new tests
  register_web_response_tests();
  register_web_request_tests();
  register_mock_tests();
  register_helper_tests();
  register_web_module_interface_tests();
  register_web_platform_interface_tests();
  register_testing_platform_provider_json_tests();

  UNITY_END();
  return 0;
}
#else
void setup() {
  UNITY_BEGIN();

  // Register and run all test groups
  register_core_types_tests();
  register_route_variant_tests();
  register_web_module_types_tests();
  // Re-enable web_response tests with selectively disabled new tests
  register_web_response_tests();
  register_web_request_tests();
  register_mock_tests();
  register_helper_tests();
  register_web_module_interface_tests();
  register_web_platform_interface_tests();
  register_testing_platform_provider_json_tests();

  UNITY_END();
}

void loop() {}
#endif