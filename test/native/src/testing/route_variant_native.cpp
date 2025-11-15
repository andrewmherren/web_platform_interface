#include "../../include/testing/route_variant_native.h"
#include "native/include/test_handler_types.h"
#include <ArduinoFake.h>
#include <interface/utils/route_variant.h>
#include <interface/web_module_interface.h>
#include <testing/mock_web_platform.h>
#include <unity.h>

// Test the native implementation of RouteVariant
// This focuses specifically on the dummy handlers used in error cases

// Simple test handlers
static void testHandler(TestRequest &req, TestResponse &res) {
  res.setContent("test", "text/plain");
}

void test_route_variant_native_dummy_web_handler() {
  // Create an ApiRoute
  ApiRoute apiRoute("/api/test", WebModule::WM_GET, testHandler,
                    AuthRequirements{});

  // Create a RouteVariant from it
  RouteVariant variant(apiRoute);

  // Get a WebRoute from it - this should use the dummy handler (tests line 56)
  const WebRoute &dummyRoute = variant.getWebRoute();

  // Actually test the dummy handler by checking route properties
  // The dummy handler is created in getWebRoute() when type != WEB_ROUTE
  // This is a static lambda defined at line 56
  TEST_ASSERT_EQUAL(WebModule::WM_GET, dummyRoute.method);
  TEST_ASSERT_EQUAL_STRING("", dummyRoute.path.c_str());

  // Note: .handler field only exists in Arduino builds, not in native
  // The handler is stored differently in native implementation

  TEST_PASS();
}

void test_route_variant_native_dummy_api_handler() {
  // Create a WebRoute
  WebRoute webRoute("/test", WebModule::WM_GET, testHandler,
                    AuthRequirements{});

  // Create a RouteVariant from it
  RouteVariant variant(webRoute);

  // Get an ApiRoute from it - this should use the dummy handler (tests line 67)
  const ApiRoute &dummyRoute = variant.getApiRoute();

  // Actually test the dummy handler by checking route properties
  // The dummy handler is created in getApiRoute() when type != API_ROUTE
  // This is a static lambda defined at line 67
  TEST_ASSERT_EQUAL(WebModule::WM_GET, dummyRoute.webRoute.method);

  // The path is '/' not an empty string in the implementation
  TEST_ASSERT_EQUAL_STRING("/", dummyRoute.webRoute.path.c_str());

  // Note: .handler field only exists in Arduino builds, not in native
  // The handler is stored differently in native implementation

  TEST_PASS();
}

void test_route_variant_native_copy_constructor() {
  // Test copying WebRoute variant
  WebRoute webRoute("/original", WebModule::WM_POST, testHandler,
                    AuthRequirements{AuthType::SESSION});
  RouteVariant original(webRoute);

  RouteVariant copy(original);

  TEST_ASSERT_TRUE(copy.isWebRoute());
  TEST_ASSERT_FALSE(copy.isApiRoute());
  const WebRoute &copiedRoute = copy.getWebRoute();
  TEST_ASSERT_EQUAL_STRING("/original", copiedRoute.path.c_str());
  TEST_ASSERT_EQUAL(WebModule::WM_POST, copiedRoute.method);
  TEST_ASSERT_EQUAL(1, copiedRoute.authRequirements.size());
  TEST_ASSERT_EQUAL(AuthType::SESSION, copiedRoute.authRequirements[0]);

  // Test copying ApiRoute variant
  ApiRoute apiRoute("/api/test", WebModule::WM_PUT, testHandler,
                    AuthRequirements{AuthType::TOKEN});
  RouteVariant apiOriginal(apiRoute);

  RouteVariant apiCopy(apiOriginal);

  TEST_ASSERT_FALSE(apiCopy.isWebRoute());
  TEST_ASSERT_TRUE(apiCopy.isApiRoute());
  const ApiRoute &copiedApiRoute = apiCopy.getApiRoute();
  TEST_ASSERT_EQUAL_STRING("/test", copiedApiRoute.webRoute.path.c_str());
  TEST_ASSERT_EQUAL(WebModule::WM_PUT, copiedApiRoute.webRoute.method);
  TEST_ASSERT_EQUAL(1, copiedApiRoute.webRoute.authRequirements.size());
  TEST_ASSERT_EQUAL(AuthType::TOKEN,
                    copiedApiRoute.webRoute.authRequirements[0]);
}

void test_route_variant_native_assignment_operator() {
  // Test assigning WebRoute variant
  WebRoute webRoute("/source", WebModule::WM_DELETE, testHandler,
                    AuthRequirements{AuthType::LOCAL_ONLY});
  RouteVariant source(webRoute);

  ApiRoute apiRoute("/api/dest", WebModule::WM_GET, testHandler);
  RouteVariant dest(apiRoute);

  // Verify dest starts as ApiRoute
  TEST_ASSERT_TRUE(dest.isApiRoute());

  // Assign WebRoute to it
  dest = source;

  // Verify dest is now a WebRoute
  TEST_ASSERT_TRUE(dest.isWebRoute());
  TEST_ASSERT_FALSE(dest.isApiRoute());
  const WebRoute &assignedRoute = dest.getWebRoute();
  TEST_ASSERT_EQUAL_STRING("/source", assignedRoute.path.c_str());
  TEST_ASSERT_EQUAL(WebModule::WM_DELETE, assignedRoute.method);
  TEST_ASSERT_EQUAL(1, assignedRoute.authRequirements.size());
  TEST_ASSERT_EQUAL(AuthType::LOCAL_ONLY, assignedRoute.authRequirements[0]);

  // Test assigning ApiRoute variant
  ApiRoute apiRoute2("/api/source2", WebModule::WM_PATCH, testHandler,
                     AuthRequirements{AuthType::TOKEN});
  RouteVariant apiSource(apiRoute2);

  WebRoute webRoute2("/dest2", WebModule::WM_GET, testHandler);
  RouteVariant webDest(webRoute2);

  // Verify webDest starts as WebRoute
  TEST_ASSERT_TRUE(webDest.isWebRoute());

  // Assign ApiRoute to it
  webDest = apiSource;

  // Verify webDest is now an ApiRoute
  TEST_ASSERT_FALSE(webDest.isWebRoute());
  TEST_ASSERT_TRUE(webDest.isApiRoute());
  const ApiRoute &assignedApiRoute = webDest.getApiRoute();
  TEST_ASSERT_EQUAL_STRING("/source2", assignedApiRoute.webRoute.path.c_str());
  TEST_ASSERT_EQUAL(WebModule::WM_PATCH, assignedApiRoute.webRoute.method);
  TEST_ASSERT_EQUAL(1, assignedApiRoute.webRoute.authRequirements.size());
  TEST_ASSERT_EQUAL(AuthType::TOKEN,
                    assignedApiRoute.webRoute.authRequirements[0]);
}

void test_route_variant_native_self_assignment() {
  // Test self-assignment doesn't break anything
  WebRoute webRoute("/self", WebModule::WM_GET, testHandler);
  RouteVariant variant(webRoute);

  variant = variant;

  TEST_ASSERT_TRUE(variant.isWebRoute());
  const WebRoute &route = variant.getWebRoute();
  TEST_ASSERT_EQUAL_STRING("/self", route.path.c_str());
  TEST_ASSERT_EQUAL(WebModule::WM_GET, route.method);
}

void test_route_variant_native_helper_functions() {
  WebRoute webRoute("/web", WebModule::WM_GET, testHandler);
  RouteVariant webVariant(webRoute);

  ApiRoute apiRoute("/api/test", WebModule::WM_POST, testHandler);
  RouteVariant apiVariant(apiRoute);

  // Test holds_alternative helper
  TEST_ASSERT_TRUE(holds_alternative<WebRoute>(webVariant));
  TEST_ASSERT_FALSE(holds_alternative<ApiRoute>(webVariant));
  TEST_ASSERT_FALSE(holds_alternative<WebRoute>(apiVariant));
  TEST_ASSERT_TRUE(holds_alternative<ApiRoute>(apiVariant));

  // Test get helper
  const WebRoute &extractedWeb = get<WebRoute>(webVariant);
  TEST_ASSERT_EQUAL_STRING("/web", extractedWeb.path.c_str());

  const ApiRoute &extractedApi = get<ApiRoute>(apiVariant);
  TEST_ASSERT_EQUAL_STRING("/test", extractedApi.webRoute.path.c_str());
}

// Test route_variant_native.cpp constructor branches (lines 11, 15, 20, 22)
void test_route_variant_native_constructor_branches() {
  // Test WebRoute constructor (line 11)
  WebRoute webRoute1("/path1", WebModule::WM_GET, testHandler);
  RouteVariant variant1(webRoute1);
  TEST_ASSERT_TRUE(variant1.isWebRoute());

  // Test ApiRoute constructor (line 15)
  ApiRoute apiRoute1("/api/path1", WebModule::WM_POST, testHandler);
  RouteVariant variant2(apiRoute1);
  TEST_ASSERT_TRUE(variant2.isApiRoute());

  // Test copy constructor with WEB_ROUTE (line 20)
  RouteVariant variant3(variant1);
  TEST_ASSERT_TRUE(variant3.isWebRoute());

  // Test copy constructor with API_ROUTE (line 22)
  RouteVariant variant4(variant2);
  TEST_ASSERT_TRUE(variant4.isApiRoute());
}

// Test assignment operator branches (lines 30, 32, 38, 40)
void test_route_variant_native_assignment_branches() {
  WebRoute webRoute("/web", WebModule::WM_GET, testHandler);
  ApiRoute apiRoute("/api/path", WebModule::WM_POST, testHandler);

  // Test delete webRoute in assignment (line 30)
  RouteVariant variant1(webRoute);
  RouteVariant variant2(apiRoute);
  variant1 = variant2; // This should delete webRoute
  TEST_ASSERT_TRUE(variant1.isApiRoute());

  // Test delete apiRoute in assignment (line 32)
  RouteVariant variant3(apiRoute);
  RouteVariant variant4(webRoute);
  variant3 = variant4; // This should delete apiRoute
  TEST_ASSERT_TRUE(variant3.isWebRoute());

  // Test new WebRoute in assignment (line 38)
  RouteVariant variant5(apiRoute);
  variant5 = variant4; // type becomes WEB_ROUTE, creates new WebRoute
  TEST_ASSERT_TRUE(variant5.isWebRoute());

  // Test new ApiRoute in assignment (line 40)
  RouteVariant variant6(webRoute);
  variant6 = variant2; // type becomes API_ROUTE, creates new ApiRoute
  TEST_ASSERT_TRUE(variant6.isApiRoute());
}

// Test destructor branches (lines 48, 50)
void test_route_variant_native_destructor_branches() {
  // Test destruction of WEB_ROUTE (line 48)
  {
    WebRoute webRoute("/web", WebModule::WM_GET, testHandler);
    RouteVariant variant(webRoute);
    TEST_ASSERT_TRUE(variant.isWebRoute());
  } // Destructor should delete webRoute here

  // Test destruction of API_ROUTE (line 50)
  {
    ApiRoute apiRoute("/api/path", WebModule::WM_POST, testHandler);
    RouteVariant variant(apiRoute);
    TEST_ASSERT_TRUE(variant.isApiRoute());
  } // Destructor should delete apiRoute here

  TEST_PASS();
}

// Test wrong-type getter branches (lines 56, 60, 67, 71)
void test_route_variant_native_wrong_type_getters() {
  WebRoute webRoute("/web", WebModule::WM_GET, testHandler);
  ApiRoute apiRoute("/api/path", WebModule::WM_POST, testHandler);

  // Test getWebRoute when type is API_ROUTE (line 56)
  RouteVariant apiVariant(apiRoute);
  const WebRoute &dummyWeb = apiVariant.getWebRoute();
  TEST_ASSERT_EQUAL_STRING("", dummyWeb.path.c_str());

  // Test getApiRoute when type is WEB_ROUTE (line 67)
  RouteVariant webVariant(webRoute);
  const ApiRoute &dummyApi = webVariant.getApiRoute();
  // Line 69 creates dummy handler, line 70 creates dummy ApiRoute with "/"
  TEST_ASSERT_EQUAL_STRING("/", dummyApi.webRoute.path.c_str());

  // Test actual correct getters (lines 63, 74)
  RouteVariant webVariant2(webRoute);
  const WebRoute &actualWeb = webVariant2.getWebRoute();
  TEST_ASSERT_EQUAL_STRING("/web", actualWeb.path.c_str());

  RouteVariant apiVariant2(apiRoute);
  const ApiRoute &actualApi = apiVariant2.getApiRoute();
  TEST_ASSERT_EQUAL_STRING("/path", actualApi.webRoute.path.c_str());
}

// Registration function
void register_route_variant_native_tests() {
  RUN_TEST(test_route_variant_native_dummy_web_handler);
  RUN_TEST(test_route_variant_native_dummy_api_handler);
  RUN_TEST(test_route_variant_native_copy_constructor);
  RUN_TEST(test_route_variant_native_assignment_operator);
  RUN_TEST(test_route_variant_native_self_assignment);
  RUN_TEST(test_route_variant_native_helper_functions);
  RUN_TEST(test_route_variant_native_constructor_branches);
  RUN_TEST(test_route_variant_native_assignment_branches);
  RUN_TEST(test_route_variant_native_destructor_branches);
  RUN_TEST(test_route_variant_native_wrong_type_getters);
}
