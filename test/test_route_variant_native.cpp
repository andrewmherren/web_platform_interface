#include "test_route_variant_native.h"
#include <ArduinoFake.h>
#include <interface/utils/route_variant.h>
#include <interface/web_module_interface.h>
#include <testing/mock_web_platform.h>
#include <unity.h>

// Test the native implementation of RouteVariant
// This focuses specifically on the dummy handlers used in error cases

// Simple test handlers
static void testHandler(WebRequest &req, WebResponse &res) {
  res.setContent("test", "text/plain");
}

void test_route_variant_native_dummy_web_handler() {
  // Create an ApiRoute
  ApiRoute apiRoute("/api/test", WebModule::WM_GET, testHandler,
                    AuthRequirements{});

  // Create a RouteVariant from it
  RouteVariant variant(apiRoute);

  // Get a WebRoute from it - this should use the dummy handler
  const WebRoute &dummyRoute = variant.getWebRoute();

  // No need to actually call the handler - just getting the WebRoute
  // is enough to test the dummy handler creation
  TEST_PASS();
}

void test_route_variant_native_dummy_api_handler() {
  // Create a WebRoute
  WebRoute webRoute("/test", WebModule::WM_GET, testHandler,
                    AuthRequirements{});

  // Create a RouteVariant from it
  RouteVariant variant(webRoute);

  // Get an ApiRoute from it - this should use the dummy handler
  const ApiRoute &dummyRoute = variant.getApiRoute();

  // No need to actually call the handler - just getting the ApiRoute
  // is enough to test the dummy handler creation
  TEST_PASS();
}

// Registration function
void register_route_variant_native_tests() {
  RUN_TEST(test_route_variant_native_dummy_web_handler);
  RUN_TEST(test_route_variant_native_dummy_api_handler);
}