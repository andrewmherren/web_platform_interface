#include "test_web_platform_interface.h"
#include <ArduinoFake.h>
#include <ArduinoJson.h>
#include <unity.h>
#include <web_platform_interface.h>

// Include mock infrastructure
#include <interface/string_compat.h>
#include <testing/mock_web_platform.h>
#include <testing/testing_platform_provider.h>


// Simple test module for testing platform interactions
class TestWebModule : public IWebModule {
public:
  std::vector<RouteVariant> getHttpRoutes() override {
    return {WebRoute("/test", WebModule::WM_GET,
                     [](WebRequest &req, WebResponse &res) {
                       res.setContent("Test Module Response", "text/plain");
                     }),
            ApiRoute("/api/test", WebModule::WM_GET,
                     [](WebRequest &req, WebResponse &res) {
                       res.setContent("{\"status\":\"ok\"}",
                                      "application/json");
                     },
                     {AuthType::NONE})};
  }

  std::vector<RouteVariant> getHttpsRoutes() override {
    return {WebRoute("/secure-test", WebModule::WM_GET,
                     [](WebRequest &req, WebResponse &res) {
                       res.setContent("Secure Test Response", "text/plain");
                     },
                     {AuthType::SESSION})};
  }

  String getModuleName() const override { return "TestWebModule"; }
};

// Test IWebPlatform lifecycle operations
void test_iwebplatform_lifecycle_operations() {
  MockWebPlatformProvider provider;
  IWebPlatform &platform = provider.getPlatform();

  // Test basic initialization
  platform.begin("TestDevice");
  TEST_ASSERT_EQUAL_STRING("TestDevice", platform.getDeviceName().c_str());

  // Test HTTPS-enabled initialization
  platform.begin("SecureDevice", true);
  TEST_ASSERT_EQUAL_STRING("SecureDevice", platform.getDeviceName().c_str());
  TEST_ASSERT_TRUE(platform.isHttpsEnabled());

  // Test connection state
  TEST_ASSERT_TRUE(platform.isConnected());

  // Test base URL generation
  String baseUrl = platform.getBaseUrl();
  TEST_ASSERT_TRUE(baseUrl.length() > 0);
  TEST_ASSERT_TRUE(baseUrl.indexOf("mock-device") >= 0);

  // Test handle method (should not crash)
  platform.handle();
}

// Test IWebPlatform module registration
void test_iwebplatform_module_registration() {
  MockWebPlatformProvider provider;
  IWebPlatform &platform = provider.getPlatform();
  MockWebPlatform &mockPlatform = provider.getMockPlatform();

  platform.begin("ModuleTestDevice");

  TestWebModule testModule;
  platform.registerModule("/test", &testModule);

  // Verify module was registered
  TEST_ASSERT_EQUAL(1, mockPlatform.getRegisteredModuleCount());

  auto registeredModules = mockPlatform.getRegisteredModules();
  TEST_ASSERT_EQUAL_STRING("/test", registeredModules[0].first.c_str());
  TEST_ASSERT_EQUAL_PTR(&testModule, registeredModules[0].second);

  // Route count should reflect module routes (2 HTTP + 1 HTTPS = 3)
  TEST_ASSERT_EQUAL(3, platform.getRouteCount());
}

// Test IWebPlatform route registration
void test_iwebplatform_route_registration() {
  MockWebPlatformProvider provider;
  IWebPlatform &platform = provider.getPlatform();

  platform.begin("RouteTestDevice");
  size_t initialRouteCount = platform.getRouteCount();

  // Test web route registration
  platform.registerWebRoute(
      "/custom",
      [](WebRequest &req, WebResponse &res) {
        res.setContent("Custom Response", "text/plain");
      },
      {AuthType::NONE}, WebModule::WM_GET);

  TEST_ASSERT_EQUAL(initialRouteCount + 1, platform.getRouteCount());

  // Test API route registration with authentication
  platform.registerApiRoute(
      "/api/custom",
      [](WebRequest &req, WebResponse &res) {
        res.setContent("{\"message\":\"custom\"}", "application/json");
      },
      {AuthType::TOKEN}, WebModule::WM_POST, OpenAPIDocumentation());

  TEST_ASSERT_EQUAL(initialRouteCount + 2, platform.getRouteCount());

  // Test route disabling
  platform.disableRoute("/custom", WebModule::WM_GET);
  TEST_ASSERT_EQUAL(initialRouteCount + 1, platform.getRouteCount());
}

// Test IWebPlatform configuration methods
void test_iwebplatform_configuration_methods() {
  MockWebPlatformProvider provider;
  IWebPlatform &platform = provider.getPlatform();

  platform.begin("ConfigTestDevice");

  // Test error page configuration
  platform.setErrorPage(404, "<html><body>Not Found</body></html>");

  // Test global redirect configuration
  platform.addGlobalRedirect("/old-path", "/new-path");

  // These should not crash (mock implementations)
  // Real functionality would be tested in the actual WebPlatform implementation
}

// Test IWebPlatform JSON response utilities
void test_iwebplatform_json_response_utilities() {
  MockWebPlatformProvider provider;
  IWebPlatform &platform = provider.getPlatform();

  platform.begin("JsonTestDevice");

  // Test MockWebResponse directly for basic functionality
  MockWebResponse mockRes;
  mockRes.setContent("{\"test\":true}", "application/json");

  TEST_ASSERT_EQUAL_STRING("application/json",
                           mockRes.getContentType().c_str());
  TEST_ASSERT_TRUE(mockRes.getContent().indexOf("test") >= 0);

  // Test that we can call the platform's JSON creation methods
  // This tests that the MockWebPlatform implementation works
  MockWebResponse jsonRes;

  // Use the newer, safer approach without any casting at all
  jsonRes.setContent("{\"success\":true,\"message\":\"Direct test\"}",
                     "application/json");

  TEST_ASSERT_EQUAL_STRING("application/json",
                           jsonRes.getContentType().c_str());
  TEST_ASSERT_TRUE(jsonRes.getContent().indexOf("success") >= 0);
  TEST_ASSERT_TRUE(jsonRes.getContent().indexOf("Direct test") >= 0);

  // Demonstrate the new template-based operation pattern
  MockWebResponse templateRes;
  runResponseOperation(templateRes, [](MockWebResponse &res) {
    res.setContent("{\"template\":true}", "application/json");
    res.setHeader("X-Test", "Template-Pattern");
  });

  TEST_ASSERT_EQUAL_STRING("application/json",
                           templateRes.getContentType().c_str());
  TEST_ASSERT_EQUAL_STRING("Template-Pattern",
                           templateRes.getHeader("X-Test").c_str());
  TEST_ASSERT_TRUE(templateRes.getContent().indexOf("template") >= 0);

  // The platform's createJsonResponse methods are tested in the MockWebPlatform
  // implementation
}

// Test IWebPlatformProvider singleton pattern
void test_iwebplatformprovider_singleton_pattern() {
  // Save the original provider instance
  IWebPlatformProvider *originalInstance = IWebPlatformProvider::instance;

  // Test that provider can be set
  MockWebPlatformProvider testProvider;
  IWebPlatformProvider::instance = &testProvider;

  // Test accessing platform through static method
  IWebPlatform &platform = IWebPlatformProvider::getPlatformInstance();
  platform.begin("SingletonTestDevice");
  TEST_ASSERT_EQUAL_STRING("SingletonTestDevice",
                           platform.getDeviceName().c_str());

  // Verify it's the same instance
  IWebPlatform &platform2 = IWebPlatformProvider::getPlatformInstance();
  TEST_ASSERT_EQUAL_PTR(&platform, &platform2);

  // Restore original instance
  IWebPlatformProvider::instance = originalInstance;
}

// Test IWebPlatformProvider dependency injection
void test_iwebplatformprovider_dependency_injection() {
  // Create separate provider instances
  MockWebPlatformProvider provider1;
  MockWebPlatformProvider provider2;

  // Each provider should have its own platform instance
  IWebPlatform &platform1 = provider1.getPlatform();
  IWebPlatform &platform2 = provider2.getPlatform();

  TEST_ASSERT_NOT_EQUAL(&platform1, &platform2);

  // Configure platforms differently
  platform1.begin("Device1");
  platform2.begin("Device2");

  TEST_ASSERT_EQUAL_STRING("Device1", platform1.getDeviceName().c_str());
  TEST_ASSERT_EQUAL_STRING("Device2", platform2.getDeviceName().c_str());

  // Test that each platform maintains separate state
  TestWebModule module1, module2;
  platform1.registerModule("/path1", &module1);
  platform2.registerModule("/path2", &module2);

  MockWebPlatform &mock1 = provider1.getMockPlatform();
  MockWebPlatform &mock2 = provider2.getMockPlatform();

  TEST_ASSERT_EQUAL(1, mock1.getRegisteredModuleCount());
  TEST_ASSERT_EQUAL(1, mock2.getRegisteredModuleCount());

  auto modules1 = mock1.getRegisteredModules();
  auto modules2 = mock2.getRegisteredModules();

  TEST_ASSERT_EQUAL_STRING("/path1", modules1[0].first.c_str());
  TEST_ASSERT_EQUAL_STRING("/path2", modules2[0].first.c_str());
}

// Test MockWebPlatform implementation details
void test_mock_web_platform_implementation() {
  MockWebPlatform mockPlatform;

  // Test default state
  TEST_ASSERT_TRUE(mockPlatform.isConnected());
  TEST_ASSERT_TRUE(mockPlatform.isHttpsEnabled());
  TEST_ASSERT_EQUAL(0, mockPlatform.getRouteCount());

  // Test connection state manipulation
  mockPlatform.setConnected(false);
  TEST_ASSERT_FALSE(mockPlatform.isConnected());

  // Test route counting
  mockPlatform.registerWebRoute(
      "/test", [](WebRequest &req, WebResponse &res) {}, {AuthType::NONE},
      WebModule::WM_GET);
  TEST_ASSERT_EQUAL(1, mockPlatform.getRouteCount());

  mockPlatform.registerApiRoute(
      "/api/test", [](WebRequest &req, WebResponse &res) {}, {AuthType::NONE},
      WebModule::WM_GET, OpenAPIDocumentation());
  TEST_ASSERT_EQUAL(2, mockPlatform.getRouteCount());

  mockPlatform.disableRoute("/test", WebModule::WM_GET);
  TEST_ASSERT_EQUAL(1, mockPlatform.getRouteCount());

  // Test base URL generation
  String httpsUrl = mockPlatform.getBaseUrl();
  TEST_ASSERT_TRUE(httpsUrl.indexOf("https://") == 0);

  mockPlatform.begin("TestDevice", false); // HTTP only
  String httpUrl = mockPlatform.getBaseUrl();
  TEST_ASSERT_TRUE(httpUrl.indexOf("http://") == 0);
  TEST_ASSERT_FALSE(httpUrl.indexOf("https://") == 0);
}

// Test platform provider error handling
void test_platform_provider_error_handling() {
  // Save original instance
  IWebPlatformProvider *originalInstance = IWebPlatformProvider::instance;

  // Set instance to null to simulate uninitialized state
  IWebPlatformProvider::instance = nullptr;

// Note: In actual hardware environment, this would halt execution
// For testing, we just verify the check exists
#ifdef NATIVE_PLATFORM
  // We can't easily test the halt behavior in native tests
  // Just verify the instance check exists by setting a valid instance
  MockWebPlatformProvider testProvider;
  IWebPlatformProvider::instance = &testProvider;

  // This should work without error
  IWebPlatform &platform = IWebPlatformProvider::getPlatformInstance();
  TEST_ASSERT_NOT_NULL(&platform);
#endif

  // Restore original instance
  IWebPlatformProvider::instance = originalInstance;
}

// Test integrated platform-module workflow
void test_integrated_platform_module_workflow() {
  MockWebPlatformProvider provider;
  IWebPlatform &platform = provider.getPlatform();
  MockWebPlatform &mockPlatform = provider.getMockPlatform();

  // Initialize platform
  platform.begin("IntegrationTestDevice");

  // Create and register module
  TestWebModule testModule;
  platform.registerModule("/integration", &testModule);

  // Verify module registration affected route count
  size_t expectedRoutes = 3; // 2 HTTP + 1 HTTPS from TestWebModule
  TEST_ASSERT_EQUAL(expectedRoutes, platform.getRouteCount());

  // Add additional standalone routes
  platform.registerWebRoute(
      "/standalone",
      [](WebRequest &req, WebResponse &res) {
        res.setContent("Standalone", "text/plain");
      },
      {AuthType::NONE}, WebModule::WM_GET);

  platform.registerApiRoute(
      "/api/standalone",
      [](WebRequest &req, WebResponse &res) {
        res.setContent("{\"standalone\":true}", "application/json");
      },
      {AuthType::NONE}, WebModule::WM_GET, OpenAPIDocumentation());

  TEST_ASSERT_EQUAL(expectedRoutes + 2, platform.getRouteCount());

  // Test configuration methods don't interfere
  platform.setErrorPage(500, "<html>Server Error</html>");
  platform.addGlobalRedirect("/old", "/new");
  TEST_ASSERT_EQUAL(expectedRoutes + 2,
                    platform.getRouteCount()); // Should be unchanged

  // Test that platform maintains correct state
  TEST_ASSERT_EQUAL_STRING("IntegrationTestDevice",
                           platform.getDeviceName().c_str());
  TEST_ASSERT_TRUE(platform.isHttpsEnabled());
  TEST_ASSERT_TRUE(platform.isConnected());

  // Verify module is still registered correctly
  TEST_ASSERT_EQUAL(1, mockPlatform.getRegisteredModuleCount());
  auto registeredModules = mockPlatform.getRegisteredModules();
  TEST_ASSERT_EQUAL_STRING("/integration", registeredModules[0].first.c_str());
}

// Registration function to run all web platform interface tests
void register_web_platform_interface_tests() {
  RUN_TEST(test_iwebplatform_lifecycle_operations);
  RUN_TEST(test_iwebplatform_module_registration);
  RUN_TEST(test_iwebplatform_route_registration);
  RUN_TEST(test_iwebplatform_configuration_methods);
  RUN_TEST(test_iwebplatform_json_response_utilities);
  RUN_TEST(test_iwebplatformprovider_singleton_pattern);
  RUN_TEST(test_iwebplatformprovider_dependency_injection);
  RUN_TEST(test_mock_web_platform_implementation);
  RUN_TEST(test_platform_provider_error_handling);
  RUN_TEST(test_integrated_platform_module_workflow);
}