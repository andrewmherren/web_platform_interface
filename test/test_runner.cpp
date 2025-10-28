#include <ArduinoFake.h>
#include <unity.h>

#include "include/test_helpers.h"
// Include all test header files
#include "include/interface/test_core_types.h"
#include "include/interface/test_string_compat.h"
#include "include/interface/test_web_module_interface.h"
#include "include/interface/test_web_module_types.h"
#include "include/interface/test_web_platform_interface.h"
#include "include/interface/test_web_request.h"
#include "include/interface/test_web_response.h"
#include "include/interface/utils/test_route_variant.h"
#include "include/testing/test_mock_web_platform.h"
#include "include/testing/test_mocks.h"
#include "include/testing/test_route_variant_native.h"
#include "include/testing/test_testing_platform_provider.h"
#include "include/testing/test_testing_platform_provider_json.h"


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
  register_route_variant_native_tests();
  register_web_module_types_tests();
  register_web_response_tests();
  register_web_request_tests();
  register_mock_tests();
  register_helper_tests();
  register_string_compat_tests();
  register_web_module_interface_tests();
  register_web_platform_interface_tests();
  register_testing_platform_provider_json_tests();
  register_testing_platform_provider_tests(); // Register testing platform
                                              // provider tests
  register_mock_web_platform_tests(); // Register our new mock platform tests

  UNITY_END();
  return 0;
}
#else
void setup() {
  UNITY_BEGIN();

  // Register and run all test groups
  register_core_types_tests();
  register_route_variant_tests();
  register_route_variant_native_tests();
  register_web_module_types_tests();
  register_web_response_tests();
  register_web_request_tests();
  register_mock_tests();
  register_helper_tests();
  register_string_compat_tests();
  register_web_module_interface_tests();
  register_web_platform_interface_tests();
  register_testing_platform_provider_json_tests();
  register_testing_platform_provider_tests(); // Register testing platform
                                              // provider tests
  register_mock_web_platform_tests(); // Register our new mock platform tests

  UNITY_END();
}

void loop() {}
#endif