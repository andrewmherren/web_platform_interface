#include "test_mock_web_platform.h"
#include "interface/openapi_types.h"
#include <ArduinoFake.h>
#include <testing/testing_platform_provider.h>
#include <unity.h>
#include <web_platform_interface.h>

// Test basic functionality of the mock platform
void test_mock_web_platform_basics() {
  MockWebPlatformProvider provider;
  MockWebPlatform &platform = provider.getMockPlatform();

  // Test initialization
  platform.begin("TestDevice");
  TEST_ASSERT_EQUAL_STRING("TestDevice", platform.getDeviceName().c_str());

  // Test HTTPS configuration
  TEST_ASSERT_TRUE(platform.isHttpsEnabled());
  platform.begin("SecureDevice", false);
  TEST_ASSERT_FALSE(platform.isHttpsEnabled());
  TEST_ASSERT_EQUAL_STRING("SecureDevice", platform.getDeviceName().c_str());
}

// Test route registration and warnings
void test_mock_web_platform_routes() {
  MockWebPlatformProvider provider;
  MockWebPlatform &platform = provider.getMockPlatform();

  // Capture warnings
  bool warningEmitted = false;
  String capturedWarning;
  platform.onWarn([&warningEmitted, &capturedWarning](const String &msg) {
    warningEmitted = true;
    capturedWarning = msg;
  });

  // Register normal route (no warning)
  platform.registerWebRoute(
      "/normal", [](WebRequest &req, WebResponse &res) {}, {AuthType::NONE},
      WebModule::WM_GET);
  TEST_ASSERT_FALSE(warningEmitted);

  // Route count should be 1
  TEST_ASSERT_EQUAL(1, platform.getRouteCount());

  // Register API route with WebRoute (should warn)
  platform.registerWebRoute(
      "/api/test", [](WebRequest &req, WebResponse &res) {}, {AuthType::TOKEN},
      WebModule::WM_POST);
  TEST_ASSERT_TRUE(warningEmitted);
  TEST_ASSERT_TRUE(capturedWarning.indexOf("/api/") >= 0);

  // Route count should be 2
  TEST_ASSERT_EQUAL(2, platform.getRouteCount());

  // Reset warning
  warningEmitted = false;
  capturedWarning = "";

  // Register proper API route (no warning)
  platform.registerApiRoute(
      "/api/proper", [](WebRequest &req, WebResponse &res) {},
      {AuthType::TOKEN}, WebModule::WM_PUT, OpenAPIDocumentation());
  TEST_ASSERT_FALSE(warningEmitted);

  // Route count should be 3
  TEST_ASSERT_EQUAL(3, platform.getRouteCount());

  // Disable a route
  platform.disableRoute("/normal", WebModule::WM_GET);

  // Route count should now be 2
  TEST_ASSERT_EQUAL(2, platform.getRouteCount());
}

// Test module registration and handling
void test_mock_web_platform_modules() {
  MockWebPlatformProvider provider;
  MockWebPlatform &platform = provider.getMockPlatform();

  // Create a test module
  class TestModule : public IWebModule {
  private:
    bool handleCalled = false;

  public:
    std::vector<RouteVariant> getHttpRoutes() override {
      return {WebRoute("/test", WebModule::WM_GET,
                       [](WebRequest &req, WebResponse &res) {})};
    }

    std::vector<RouteVariant> getHttpsRoutes() override { return {}; }

    String getModuleName() const override { return "TestModule"; }

    void begin() override {
      // Initialize module
    }

    void handle() override { handleCalled = true; }

    bool wasHandleCalled() const { return handleCalled; }
  };

  // Create module instance
  TestModule module;

  // Register module with platform
  platform.registerModule("/test", &module);

  // Call platform handle (should call module's handle)
  platform.handle();

  // Verify module's handle was called
  TEST_ASSERT_TRUE(module.wasHandleCalled());
}

// Test JSON response creation
void test_mock_web_platform_json() {
  MockWebPlatformProvider provider;
  MockWebPlatform &platform = provider.getMockPlatform();
  WebResponse response;

  // Test JSON object creation
  platform.createJsonResponse(response, [](JsonObject &obj) {
    obj["status"] = "success";
    obj["code"] = 200;
  });

  // Verify response content
  TEST_ASSERT_TRUE(response.getContent().indexOf("success") >= 0);
  TEST_ASSERT_TRUE(response.getContent().indexOf("200") >= 0);

  // Test JSON array creation
  platform.createJsonArrayResponse(response, [](JsonArray &arr) {
    JsonObject obj1 = arr.createNestedObject();
    obj1["name"] = "Item 1";

    JsonObject obj2 = arr.createNestedObject();
    obj2["name"] = "Item 2";
  });

  // Verify array response
  TEST_ASSERT_TRUE(response.getContent().indexOf("Item 1") >= 0);
  TEST_ASSERT_TRUE(response.getContent().indexOf("Item 2") >= 0);
}

// Test parameter handling in mock request
void test_mock_web_platform_params() {
  MockWebPlatformProvider provider;

  // Create a MockWebRequest - the param methods are in this class, not in
  // MockWebPlatform
  MockWebRequest mockRequest;

  // Set mock parameters
  mockRequest.setParam("id", "123");
  mockRequest.setParam("name", "test");

  // Test getParam - this tests the functionality in mock_web_platform.h
  TEST_ASSERT_EQUAL_STRING("123", mockRequest.getParam("id").c_str());
  TEST_ASSERT_EQUAL_STRING("test", mockRequest.getParam("name").c_str());

  // Test missing parameter
  TEST_ASSERT_EQUAL_STRING("", mockRequest.getParam("nonexistent").c_str());

  // Test getAllParams
  auto allParams = mockRequest.getAllParams();
  TEST_ASSERT_EQUAL(2, allParams.size());
  TEST_ASSERT_EQUAL_STRING("123", allParams["id"].c_str());
  TEST_ASSERT_EQUAL_STRING("test", allParams["name"].c_str());

  // Test module base path
  mockRequest.setModuleBasePath("/test_module");
  TEST_ASSERT_EQUAL_STRING("/test_module",
                           mockRequest.getModuleBasePath().c_str());
}

// Register all mock platform tests
void register_mock_web_platform_tests() {
  RUN_TEST(test_mock_web_platform_basics);
  RUN_TEST(test_mock_web_platform_routes);
  RUN_TEST(test_mock_web_platform_modules);
  RUN_TEST(test_mock_web_platform_json);
  RUN_TEST(test_mock_web_platform_params);
}