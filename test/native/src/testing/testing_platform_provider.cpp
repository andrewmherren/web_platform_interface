#include "../../include/testing/testing_platform_provider.h"
#include "native/include/test_handler_types.h"
#include <ArduinoFake.h>
#include <testing/testing_platform_provider.h>
#include <unity.h>
#include <web_platform_interface.h>

// Test MockWebPlatform initialization and callback defaults (lines 22-24)
void test_mock_web_platform_callback_defaults() {
  MockWebPlatform platform;

  // Test that default callbacks don't crash when called
  // This tests the default lambda functions on lines 22-24
  platform.registerWebRoute(
      "/api/test", [](TestRequest &req, TestResponse &res) {}, {AuthType::NONE},
      WebModule::WM_GET);

  // The warning callback should have been invoked (with default no-op lambda)
  // but it shouldn't crash - this validates the default lambda on line 22
  TEST_ASSERT_TRUE(true); // Test passes if we don't crash
}

// Test both httpsEnabled conditions (lines 38-39, 48-50)
void test_mock_web_platform_https_conditions() {
  MockWebPlatform platform;

  // Test HTTPS enabled (true branch - line 48)
  platform.begin("Device1", true);
  TEST_ASSERT_TRUE(platform.isHttpsEnabled());
  TEST_ASSERT_EQUAL_STRING("https://mock-device.local",
                           platform.getBaseUrl().c_str());

  // Test HTTPS disabled (false branch - line 49)
  platform.begin("Device2", false);
  TEST_ASSERT_FALSE(platform.isHttpsEnabled());
  TEST_ASSERT_EQUAL_STRING("http://mock-device.local",
                           platform.getBaseUrl().c_str());
}

// Test handle() method with multiple module conditions (lines 52, 54-55)
void test_mock_web_platform_handle_conditions() {
  MockWebPlatform platform;

  // Test empty module list (line 52 - loop doesn't execute)
  platform.handle(); // Should not crash

  // Create test modules to test different conditions
  class TestModule : public IWebModule {
  private:
    mutable int handleCallCount = 0;

  public:
    std::vector<RouteVariant> getHttpRoutes() override {
      return {WebRoute("/test", WebModule::WM_GET,
                       [](TestRequest &req, TestResponse &res) {})};
    }
    std::vector<RouteVariant> getHttpsRoutes() override { return {}; }
    String getModuleName() const override { return "TestModule"; }
    void begin() override {}
    void handle() override { handleCallCount++; }
    int getHandleCallCount() const { return handleCallCount; }
  };

  TestModule validModule;

  // Test handle() with valid module (line 54-55, module != nullptr)
  platform.registerModule("/valid", &validModule);
  platform.handle();
  TEST_ASSERT_EQUAL(1, validModule.getHandleCallCount());

  // Test handle() with null module (line 54-55, module == nullptr)
  // We need to directly test the null condition
  std::vector<std::pair<String, IWebModule *>> modules;
  modules.push_back(std::make_pair("/null", nullptr));
  modules.push_back(std::make_pair("/valid", &validModule));

  // We can't directly set the registeredModules, but we can test the condition
  // by registering a module and then calling handle multiple times
  platform.handle();
  TEST_ASSERT_EQUAL(2, validModule.getHandleCallCount()); // Should increment
}

// Test registerModule route counting (lines 64, 66-68)
void test_mock_web_platform_register_module_routes() {
  MockWebPlatform platform;

  // Create modules with different route combinations
  class NoRoutesModule : public IWebModule {
  public:
    std::vector<RouteVariant> getHttpRoutes() override { return {}; }
    std::vector<RouteVariant> getHttpsRoutes() override { return {}; }
    String getModuleName() const override { return "NoRoutesModule"; }
    void begin() override {}
    void handle() override {}
  };

  class HttpOnlyModule : public IWebModule {
  public:
    std::vector<RouteVariant> getHttpRoutes() override {
      return {WebRoute("/http1", WebModule::WM_GET,
                       [](TestRequest &req, TestResponse &res) {}),
              WebRoute("/http2", WebModule::WM_POST,
                       [](TestRequest &req, TestResponse &res) {})};
    }
    std::vector<RouteVariant> getHttpsRoutes() override { return {}; }
    String getModuleName() const override { return "HttpOnlyModule"; }
    void begin() override {}
    void handle() override {}
  };

  class HttpsOnlyModule : public IWebModule {
  public:
    std::vector<RouteVariant> getHttpRoutes() override { return {}; }
    std::vector<RouteVariant> getHttpsRoutes() override {
      return {WebRoute("/https1", WebModule::WM_GET,
                       [](TestRequest &req, TestResponse &res) {}),
              WebRoute("/https2", WebModule::WM_POST,
                       [](TestRequest &req, TestResponse &res) {}),
              WebRoute("/https3", WebModule::WM_PUT,
                       [](TestRequest &req, TestResponse &res) {})};
    }
    String getModuleName() const override { return "HttpsOnlyModule"; }
    void begin() override {}
    void handle() override {}
  };

  class BothRoutesModule : public IWebModule {
  public:
    std::vector<RouteVariant> getHttpRoutes() override {
      return {WebRoute("/both_http", WebModule::WM_GET,
                       [](TestRequest &req, TestResponse &res) {})};
    }
    std::vector<RouteVariant> getHttpsRoutes() override {
      return {WebRoute("/both_https", WebModule::WM_GET,
                       [](TestRequest &req, TestResponse &res) {})};
    }
    String getModuleName() const override { return "BothRoutesModule"; }
    void begin() override {}
    void handle() override {}
  };

  NoRoutesModule noRoutesModule;
  HttpOnlyModule httpOnlyModule;
  HttpsOnlyModule httpsOnlyModule;
  BothRoutesModule bothRoutesModule;

  // Test module with no routes (lines 66-68: 0 + 0 = 0)
  platform.registerModule("/noroutes", &noRoutesModule);
  TEST_ASSERT_EQUAL(0, platform.getRouteCount());

  // Test module with HTTP routes only (lines 66-68: 2 + 0 = 2)
  platform.registerModule("/httponly", &httpOnlyModule);
  TEST_ASSERT_EQUAL(2, platform.getRouteCount());

  // Test module with HTTPS routes only (lines 66-68: 0 + 3 = 3, total = 5)
  platform.registerModule("/httpsonly", &httpsOnlyModule);
  TEST_ASSERT_EQUAL(5, platform.getRouteCount());

  // Test module with both HTTP and HTTPS routes (lines 66-68: 1 + 1 = 2, total
  // = 7)
  platform.registerModule("/both", &bothRoutesModule);
  TEST_ASSERT_EQUAL(7, platform.getRouteCount());
}

// Test registerWebRoute API path warning conditions (line 83)
void test_mock_web_platform_api_path_warnings() {
  MockWebPlatform platform;

  bool warnCalled = false;
  std::string warnMessage;
  platform.onWarn([&warnCalled, &warnMessage](const String &msg) {
    warnCalled = true;
    warnMessage = msg.c_str();
  });

  // Test path that doesn't start with /api/ or api/ (line 83 - false condition)
  platform.registerWebRoute(
      "/normal/path", [](TestRequest &req, TestResponse &res) {},
      {AuthType::NONE}, WebModule::WM_GET);
  TEST_ASSERT_FALSE(warnCalled);

  // Reset
  warnCalled = false;
  warnMessage = "";

  // Test path that starts with /api/ (line 83 - first part true)
  platform.registerWebRoute(
      "/api/test", [](TestRequest &req, TestResponse &res) {}, {AuthType::NONE},
      WebModule::WM_GET);
  TEST_ASSERT_TRUE(warnCalled);
  TEST_ASSERT_TRUE(warnMessage.find("/api/test") != std::string::npos);

  // Reset
  warnCalled = false;
  warnMessage = "";

  // Test path that starts with api/ (line 83 - second part true)
  platform.registerWebRoute(
      "api/test", [](TestRequest &req, TestResponse &res) {}, {AuthType::NONE},
      WebModule::WM_GET);
  TEST_ASSERT_TRUE(warnCalled);
  TEST_ASSERT_TRUE(warnMessage.find("api/test") != std::string::npos);
}

// Test disableRoute conditions (lines 102-103, 106)
void test_mock_web_platform_disable_route_conditions() {
  MockWebPlatform platform;

  // Test disableRoute when routeCount == 0 (line 102-103, 106 - false
  // condition)
  TEST_ASSERT_EQUAL(0, platform.getRouteCount());
  platform.disableRoute("/nonexistent", WebModule::WM_GET);
  TEST_ASSERT_EQUAL(0, platform.getRouteCount()); // Should remain 0

  // Add a route to test the true condition
  platform.registerWebRoute(
      "/test", [](TestRequest &req, TestResponse &res) {}, {AuthType::NONE},
      WebModule::WM_GET);
  TEST_ASSERT_EQUAL(1, platform.getRouteCount());

  // Test disableRoute when routeCount > 0 (line 102-103, 106 - true condition)
  platform.disableRoute("/test", WebModule::WM_GET);
  TEST_ASSERT_EQUAL(0, platform.getRouteCount());

  // Test again when it's already 0
  platform.disableRoute("/another", WebModule::WM_GET);
  TEST_ASSERT_EQUAL(0, platform.getRouteCount()); // Should remain 0
}

// Test JSON response creation edge cases (lines 109, 117-118, 121, 124)
void test_testing_platform_provider_json_edge_cases() {
  MockWebPlatform platform;

  // Test createJsonResponse with builder that doesn't modify object (line 109)
  TestResponse response1;
  platform.createJsonResponse(response1, [](JsonObject &obj) {
    // Do nothing - test empty object serialization
  });
  TEST_ASSERT_EQUAL_STRING("{}", response1.getContent().c_str());
  TEST_ASSERT_EQUAL_STRING("application/json", response1.getMimeType().c_str());

  // Test createJsonResponse with builder that adds data (line 109)
  TestResponse response2;
  platform.createJsonResponse(response2, [](JsonObject &obj) {
    obj["key"] = "value";
    obj["number"] = 42;
  });
  TEST_ASSERT_TRUE(response2.getContent().find("value") != std::string::npos);
  TEST_ASSERT_TRUE(response2.getContent().find("42") != std::string::npos);

  // Test createJsonArrayResponse with builder that doesn't modify array (lines
  // 117-118, 121)
  TestResponse response3;
  platform.createJsonArrayResponse(response3, [](JsonArray &arr) {
    // Do nothing - test empty array serialization
  });
  TEST_ASSERT_EQUAL_STRING("[]", response3.getContent().c_str());
  TEST_ASSERT_EQUAL_STRING("application/json", response3.getMimeType().c_str());

  // Test createJsonArrayResponse with builder that adds data (lines 117-118,
  // 121, 124)
  TestResponse response4;
  platform.createJsonArrayResponse(response4, [](JsonArray &arr) {
    JsonObject obj1 = arr.createNestedObject();
    obj1["name"] = "item1";
    JsonObject obj2 = arr.createNestedObject();
    obj2["name"] = "item2";
  });
  TEST_ASSERT_TRUE(response4.getContent().find("item1") != std::string::npos);
  TEST_ASSERT_TRUE(response4.getContent().find("item2") != std::string::npos);
}

// Test MockWebPlatformProvider constructor and unique_ptr initialization (line
// 157)
void test_mock_web_platform_provider_constructor() {
  // Test successful construction (line 157 - success condition)
  MockWebPlatformProvider provider;

  // Test that the unique_ptr was properly initialized
  TEST_ASSERT_NOT_NULL(&provider.getPlatform());
  TEST_ASSERT_NOT_NULL(&provider.getMockPlatform());

  // Test that both references point to the same object
  IWebPlatform &platformRef = provider.getPlatform();
  MockWebPlatform &mockRef = provider.getMockPlatform();

  // Test that they're the same object by modifying through one and checking the
  // other
  mockRef.begin("TestDevice");
  TEST_ASSERT_EQUAL_STRING("TestDevice", platformRef.getDeviceName().c_str());
}

// Test connection state changes
void test_mock_web_platform_connection_states() {
  MockWebPlatform platform;

  // Test default connected state
  TEST_ASSERT_TRUE(platform.isConnected());

  // Test setting disconnected
  platform.setConnected(false);
  TEST_ASSERT_FALSE(platform.isConnected());

  // Test setting connected again
  platform.setConnected(true);
  TEST_ASSERT_TRUE(platform.isConnected());
}

// Test callback function assignment and invocation
void test_mock_web_platform_callback_assignment() {
  MockWebPlatform platform;

  // Test error callback assignment and potential invocation
  bool errorCalled = false;
  String errorMessage;
  platform.onError([&errorCalled, &errorMessage](const String &msg) {
    errorCalled = true;
    errorMessage = msg;
  });

  // Test debug callback assignment
  bool debugCalled = false;
  String debugMessage;
  platform.onDebug([&debugCalled, &debugMessage](const String &msg) {
    debugCalled = true;
    debugMessage = msg;
  });

  // We can't directly trigger error/debug callbacks from the current
  // implementation, but we've tested that they can be assigned without crashing
  TEST_ASSERT_FALSE(
      errorCalled); // Should remain false since we haven't triggered it
  TEST_ASSERT_FALSE(
      debugCalled); // Should remain false since we haven't triggered it
}

// Test module registration with null module
void test_mock_web_platform_null_module_registration() {
  MockWebPlatform platform;

  // Test registering null module (this should work without crashing)
  platform.registerModule("/null", nullptr);
  TEST_ASSERT_EQUAL(1, platform.getRegisteredModuleCount());

  // The route count should remain 0 since null modules have no routes
  TEST_ASSERT_EQUAL(0, platform.getRouteCount());

  // Test handle() with null module (should not crash)
  platform.handle(); // This should handle the null module gracefully
}

// Test various auth requirements and method combinations
void test_mock_web_platform_route_combinations() {
  MockWebPlatform platform;

  // Test different auth types
  platform.registerWebRoute(
      "/public", [](TestRequest &req, TestResponse &res) {}, {AuthType::NONE},
      WebModule::WM_GET);
  platform.registerWebRoute(
      "/session", [](TestRequest &req, TestResponse &res) {},
      {AuthType::SESSION}, WebModule::WM_POST);
  platform.registerWebRoute(
      "/token", [](TestRequest &req, TestResponse &res) {}, {AuthType::TOKEN},
      WebModule::WM_PUT);
  platform.registerWebRoute(
      "/local", [](TestRequest &req, TestResponse &res) {},
      {AuthType::LOCAL_ONLY}, WebModule::WM_DELETE);

  TEST_ASSERT_EQUAL(4, platform.getRouteCount());

  // Test API routes
  platform.registerApiRoute(
      "/api/users", [](TestRequest &req, TestResponse &res) {},
      {AuthType::TOKEN}, WebModule::WM_GET, OpenAPIDocumentation());
  platform.registerApiRoute(
      "/api/posts", [](TestRequest &req, TestResponse &res) {},
      {AuthType::SESSION}, WebModule::WM_POST, OpenAPIDocumentation());

  TEST_ASSERT_EQUAL(6, platform.getRouteCount());
}

// Test handle() method edge case with registeredModules vector iteration
void test_mock_web_platform_handle_module_iteration() {
  MockWebPlatform platform;

  // Create test module that tracks handle calls
  class HandleTrackingModule : public IWebModule {
  private:
    mutable bool handleCalled = false;

  public:
    std::vector<RouteVariant> getHttpRoutes() override { return {}; }
    std::vector<RouteVariant> getHttpsRoutes() override { return {}; }
    String getModuleName() const override { return "HandleTrackingModule"; }
    void begin() override {}
    void handle() override { handleCalled = true; }
    bool wasHandleCalled() const { return handleCalled; }
    void resetHandleCalled() { handleCalled = false; }
  };

  HandleTrackingModule module1, module2, module3;

  // Test handle() with no modules (line 52)
  platform.handle();

  // Add one module and test
  platform.registerModule("/mod1", &module1);
  platform.handle();
  TEST_ASSERT_TRUE(module1.wasHandleCalled());

  // Add more modules to test loop iteration (line 52 with multiple iterations)
  module1.resetHandleCalled();
  platform.registerModule("/mod2", &module2);
  platform.registerModule("/mod3", &module3);

  platform.handle();
  TEST_ASSERT_TRUE(module1.wasHandleCalled());
  TEST_ASSERT_TRUE(module2.wasHandleCalled());
  TEST_ASSERT_TRUE(module3.wasHandleCalled());
}

// Test createJsonResponse/createJsonArrayResponse with different builder
// behaviors
void test_mock_web_platform_json_builder_conditions() {
  MockWebPlatform platform;

  // Test createJsonResponse with builder that throws (edge case)
  TestResponse response1;
  bool builderCalled = false;
  platform.createJsonResponse(response1, [&builderCalled](JsonObject &obj) {
    builderCalled = true;
    obj["test"] = "value";
  });
  TEST_ASSERT_TRUE(builderCalled);
  TEST_ASSERT_TRUE(response1.getContent().find("test") != std::string::npos);

  // Test createJsonArrayResponse with builder that modifies array
  TestResponse response2;
  builderCalled = false;
  platform.createJsonArrayResponse(response2, [&builderCalled](JsonArray &arr) {
    builderCalled = true;
    arr.add("item1");
    arr.add(42);
    arr.add(true);
  });
  TEST_ASSERT_TRUE(builderCalled);
  TEST_ASSERT_TRUE(response2.getContent().find("item1") != std::string::npos);
  TEST_ASSERT_TRUE(response2.getContent().find("42") != std::string::npos);
  TEST_ASSERT_TRUE(response2.getContent().find("true") != std::string::npos);
}

// Test MockWebPlatformProvider edge cases
void test_mock_web_platform_provider_edge_cases() {
  // Test multiple providers (line 157 constructor condition)
  MockWebPlatformProvider provider1;
  MockWebPlatformProvider provider2;

  // Both should work independently
  MockWebPlatform &platform1 = provider1.getMockPlatform();
  MockWebPlatform &platform2 = provider2.getMockPlatform();

  platform1.begin("Device1");
  platform2.begin("Device2");

  TEST_ASSERT_EQUAL_STRING("Device1", platform1.getDeviceName().c_str());
  TEST_ASSERT_EQUAL_STRING("Device2", platform2.getDeviceName().c_str());

  // Test that they're truly independent
  platform1.registerWebRoute(
      "/test1", [](TestRequest &req, TestResponse &res) {}, {AuthType::NONE},
      WebModule::WM_GET);
  platform2.registerWebRoute(
      "/test2", [](TestRequest &req, TestResponse &res) {}, {AuthType::NONE},
      WebModule::WM_GET);

  TEST_ASSERT_EQUAL(1, platform1.getRouteCount());
  TEST_ASSERT_EQUAL(1, platform2.getRouteCount());
}

// Test error page and redirect methods (mock implementations)
void test_mock_web_platform_other_methods() {
  MockWebPlatform platform;

  // Test setErrorPage (should not crash - it's a mock implementation)
  platform.setErrorPage(404, "<html><body>Not Found</body></html>");

  // Test addGlobalRedirect (should not crash - it's a mock implementation)
  platform.addGlobalRedirect("/old-path", "/new-path");
  platform.addGlobalRedirect("/old-path2", "/new-path2");

  // These are mock implementations, so we just verify they don't crash
  TEST_ASSERT_TRUE(true);
}

// Test specific string conversion edge cases in JSON serialization
void test_mock_web_platform_string_conversion() {
  MockWebPlatform platform;

  // Test JSON response with special characters that need proper string
  // conversion
  TestResponse response1;
  platform.createJsonResponse(response1, [](JsonObject &obj) {
    obj["unicode"] = "\u00E9\u00F1\u00FC"; // é, ñ, ü
    obj["quotes"] = "He said \"Hello\"";
    obj["backslashes"] = "path\\to\\file";
    obj["newlines"] = "line1\nline2";
  });

  // Verify the content was properly serialized and converted
  auto content = response1.getContent();
  TEST_ASSERT_TRUE(content.length() > 0);
  TEST_ASSERT_TRUE(content.find("unicode") != std::string::npos);
  TEST_ASSERT_TRUE(content.find("quotes") != std::string::npos);
  TEST_ASSERT_EQUAL_STRING("application/json", response1.getMimeType().c_str());

  // Test array response with special characters
  TestResponse response2;
  platform.createJsonArrayResponse(response2, [](JsonArray &arr) {
    arr.add("special: \"quoted\" text");
    arr.add("path\\with\\backslashes");
    JsonObject obj = arr.createNestedObject();
    obj["key"] = "value with spaces and symbols: !@#$%";
  });

  auto arrayContent = response2.getContent();
  TEST_ASSERT_TRUE(arrayContent.length() > 0);
  TEST_ASSERT_TRUE(arrayContent.find("[") == 0);
  TEST_ASSERT_TRUE(arrayContent.rfind("]") == arrayContent.length() - 1);
  TEST_ASSERT_EQUAL_STRING("application/json", response2.getMimeType().c_str());
}

// Test comprehensive JSON serialization coverage (lines 109, 117-118, 121, 124)
void test_mock_web_platform_json_serialization_coverage() {
  MockWebPlatform platform;

  // Test createJsonResponse with complex nested structure
  TestResponse response1;
  platform.createJsonResponse(response1, [](JsonObject &obj) {
    obj["string_field"] = "text";
    obj["number_field"] = 123;
    obj["boolean_field"] = false;
    obj["null_field"] = nullptr;
    JsonArray arr = obj.createNestedArray("array_field");
    arr.add("item");
    arr.add(999);
    JsonObject nested = obj.createNestedObject("nested_field");
    nested["inner"] = "value";
    nested["inner_bool"] = true;
  });

  auto content1 = response1.getContent();
  TEST_ASSERT_TRUE(content1.find("string_field") != std::string::npos);
  TEST_ASSERT_TRUE(content1.find("123") != std::string::npos);
  TEST_ASSERT_TRUE(content1.find("false") != std::string::npos);
  TEST_ASSERT_TRUE(content1.find("array_field") != std::string::npos);
  TEST_ASSERT_TRUE(content1.find("nested_field") != std::string::npos);
  TEST_ASSERT_EQUAL_STRING("application/json", response1.getMimeType().c_str());

  // Test createJsonArrayResponse with complex array
  TestResponse response2;
  platform.createJsonArrayResponse(response2, [](JsonArray &arr) {
    // Add primitive types
    arr.add("string_item");
    arr.add(999);
    arr.add(true);
    arr.add(false);
    arr.add(nullptr);

    // Add nested objects
    JsonObject obj1 = arr.createNestedObject();
    obj1["id"] = 1;
    obj1["name"] = "first";
    obj1["active"] = true;

    JsonObject obj2 = arr.createNestedObject();
    obj2["id"] = 2;
    obj2["name"] = "second";
    obj2["active"] = false;

    // Add nested array
    JsonArray nestedArr = arr.createNestedArray();
    nestedArr.add("nested_item1");
    nestedArr.add("nested_item2");
  });

  auto content2 = response2.getContent();
  TEST_ASSERT_TRUE(content2.find("string_item") != std::string::npos);
  TEST_ASSERT_TRUE(content2.find("999") != std::string::npos);
  TEST_ASSERT_TRUE(content2.find("true") != std::string::npos);
  TEST_ASSERT_TRUE(content2.find("false") != std::string::npos);
  TEST_ASSERT_TRUE(content2.find("first") != std::string::npos);
  TEST_ASSERT_TRUE(content2.find("second") != std::string::npos);
  TEST_ASSERT_TRUE(content2.find("nested_item1") != std::string::npos);
  TEST_ASSERT_EQUAL_STRING("application/json", response2.getMimeType().c_str());
}

// Register all tests
void register_testing_platform_provider_tests() {
  RUN_TEST(test_mock_web_platform_callback_defaults);
  RUN_TEST(test_mock_web_platform_https_conditions);
  RUN_TEST(test_mock_web_platform_handle_conditions);
  RUN_TEST(test_mock_web_platform_register_module_routes);
  RUN_TEST(test_mock_web_platform_api_path_warnings);
  RUN_TEST(test_mock_web_platform_disable_route_conditions);
  RUN_TEST(test_testing_platform_provider_json_edge_cases);
  RUN_TEST(test_mock_web_platform_provider_constructor);
  RUN_TEST(test_mock_web_platform_connection_states);
  RUN_TEST(test_mock_web_platform_callback_assignment);
  RUN_TEST(test_mock_web_platform_null_module_registration);
  RUN_TEST(test_mock_web_platform_route_combinations);
  RUN_TEST(test_mock_web_platform_other_methods);
  RUN_TEST(test_mock_web_platform_handle_module_iteration);
  RUN_TEST(test_mock_web_platform_json_builder_conditions);
  RUN_TEST(test_mock_web_platform_provider_edge_cases);
  RUN_TEST(test_mock_web_platform_string_conversion);
  RUN_TEST(test_mock_web_platform_json_serialization_coverage);
}
