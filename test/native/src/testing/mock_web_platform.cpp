#include "../../include/testing/mock_web_platform.h"
#include "interface/openapi_types.h"
#include "native/include/test_handler_types.h"
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
  std::string capturedWarning;
  platform.onWarn([&warningEmitted, &capturedWarning](const String &msg) {
    warningEmitted = true;
    capturedWarning = msg.c_str();
  });

  // Register normal route (no warning)
  platform.registerWebRoute(
      "/normal", [](TestRequest &req, TestResponse &res) {}, {AuthType::NONE},
      WebModule::WM_GET);
  TEST_ASSERT_FALSE(warningEmitted);

  // Route count should be 1
  TEST_ASSERT_EQUAL(1, platform.getRouteCount());

  // Register API route with WebRoute (should warn for path with leading slash)
  platform.registerWebRoute(
      "/api/test", [](TestRequest &req, TestResponse &res) {},
      {AuthType::TOKEN}, WebModule::WM_POST);
  TEST_ASSERT_TRUE(warningEmitted);
  TEST_ASSERT_TRUE(capturedWarning.find("/api/") != std::string::npos);

  // Route count should be 2
  TEST_ASSERT_EQUAL(2, platform.getRouteCount());

  // Reset warning
  warningEmitted = false;
  capturedWarning = "";

  // Test the second path condition: path starts with 'api/' (no leading slash)
  platform.registerWebRoute(
      "api/noprefix", [](TestRequest &req, TestResponse &res) {},
      {AuthType::TOKEN}, WebModule::WM_GET);
  TEST_ASSERT_TRUE(warningEmitted);
  TEST_ASSERT_TRUE(capturedWarning.find("api/noprefix") != std::string::npos);

  // Route count should be 3
  TEST_ASSERT_EQUAL(3, platform.getRouteCount());

  // Reset warning
  warningEmitted = false;
  capturedWarning = "";

  // Register proper API route (no warning)
  platform.registerApiRoute(
      "/api/proper", [](TestRequest &req, TestResponse &res) {},
      {AuthType::TOKEN}, WebModule::WM_PUT, OpenAPIDocumentation());
  TEST_ASSERT_FALSE(warningEmitted);

  // Route count should be 4
  TEST_ASSERT_EQUAL(4, platform.getRouteCount());

  // Register proper API route (no warning)
  platform.registerApiRoute(
      "/api/proper", [](TestRequest &req, TestResponse &res) {},
      {AuthType::TOKEN}, WebModule::WM_PUT, OpenAPIDocumentation());
  TEST_ASSERT_FALSE(warningEmitted);

  // Route count should now be 5
  TEST_ASSERT_EQUAL(5, platform.getRouteCount());

  // Disable a route when routeCount > 0
  platform.disableRoute("/normal", WebModule::WM_GET);

  // Route count should now be 5
  TEST_ASSERT_EQUAL(4, platform.getRouteCount());

  // Disable all remaining routes to test boundary condition
  platform.disableRoute("/api/test", WebModule::WM_POST);
  TEST_ASSERT_EQUAL(3, platform.getRouteCount());

  platform.disableRoute("api/noprefix", WebModule::WM_GET);
  TEST_ASSERT_EQUAL(2, platform.getRouteCount());

  platform.disableRoute("/api/proper", WebModule::WM_PUT);
  TEST_ASSERT_EQUAL(1, platform.getRouteCount());

  // Test disabling when routeCount == 0 (should remain 0)
  platform.disableRoute("/nonexistent", WebModule::WM_GET);
  TEST_ASSERT_EQUAL(0, platform.getRouteCount());
}

// Test module registration and handling
void test_mock_web_platform_modules() {
  MockWebPlatformProvider provider;
  MockWebPlatform &platform = provider.getMockPlatform();

  // Create a test module
  class TestModule : public IWebModule {
  private:
    bool handleCalled = false;
    int handleCount = 0;

  public:
    std::vector<RouteVariant> getHttpRoutes() override {
      return {WebRoute("/test", WebModule::WM_GET,
                       [](TestRequest &req, TestResponse &res) {})};
    }

    std::vector<RouteVariant> getHttpsRoutes() override {
      return {WebRoute("/secure", WebModule::WM_GET,
                       [](TestRequest &req, TestResponse &res) {})};
    }

    String getModuleName() const override { return "TestModule"; }

    void begin() override {
      // Initialize module
    }

    void handle() override {
      handleCalled = true;
      handleCount++;
    }

    bool wasHandleCalled() const { return handleCalled; }
    int getHandleCount() const { return handleCount; }
  };

  // Create multiple module instances
  TestModule module1;
  TestModule module2;

  // Test module registration
  TEST_ASSERT_EQUAL(0, platform.getRegisteredModuleCount());

  // Register first module with platform (with routes)
  platform.registerModule("/test1", &module1);
  TEST_ASSERT_EQUAL(1, platform.getRegisteredModuleCount());
  TEST_ASSERT_EQUAL(2, platform.getRouteCount()); // 1 HTTP + 1 HTTPS route

  // Register second module with platform
  platform.registerModule("/test2", &module2);
  TEST_ASSERT_EQUAL(2, platform.getRegisteredModuleCount());
  TEST_ASSERT_EQUAL(4, platform.getRouteCount()); // 2x (1 HTTP + 1 HTTPS route)

  // Call platform handle (should call both module's handle methods)
  platform.handle();

  // Verify both modules' handle methods were called
  TEST_ASSERT_TRUE(module1.wasHandleCalled());
  TEST_ASSERT_TRUE(module2.wasHandleCalled());
  TEST_ASSERT_EQUAL(1, module1.getHandleCount());
  TEST_ASSERT_EQUAL(1, module2.getHandleCount());

  // Call handle again to verify count increments
  platform.handle();
  TEST_ASSERT_EQUAL(2, module1.getHandleCount());
  TEST_ASSERT_EQUAL(2, module2.getHandleCount());

  // Test accessing registered modules directly
  auto modules = platform.getRegisteredModules();
  TEST_ASSERT_EQUAL(2, modules.size());
  TEST_ASSERT_EQUAL_STRING("/test1", modules[0].first.c_str());
  TEST_ASSERT_EQUAL_STRING("/test2", modules[1].first.c_str());
}

// Test JSON response creation
void test_mock_web_platform_json() {
  MockWebPlatformProvider provider;
  MockWebPlatform &platform = provider.getMockPlatform();
  TestResponse response;

  // Test JSON object creation
  platform.createJsonResponse(response, [](JsonObject &obj) {
    obj["status"] = "success";
    obj["code"] = 200;
  });

  // Verify response content
  TEST_ASSERT_TRUE(response.getContent().find("success") != std::string::npos);
  TEST_ASSERT_TRUE(response.getContent().find("200") != std::string::npos);

  // Test JSON array creation
  platform.createJsonArrayResponse(response, [](JsonArray &arr) {
    JsonObject obj1 = arr.createNestedObject();
    obj1["name"] = "Item 1";

    JsonObject obj2 = arr.createNestedObject();
    obj2["name"] = "Item 2";
  });

  // Verify array response
  TEST_ASSERT_TRUE(response.getContent().find("Item 1") != std::string::npos);
  TEST_ASSERT_TRUE(response.getContent().find("Item 2") != std::string::npos);
}

// Test callback functions (debug, warn, error)
void test_mock_web_platform_callbacks() {
  MockWebPlatformProvider provider;
  MockWebPlatform &platform = provider.getMockPlatform();

  // Test variables to capture callback invocations
  bool warnCalled = false;
  bool errorCalled = false;
  bool debugCalled = false;

  std::string warnMessage;
  std::string errorMessage;
  std::string debugMessage;

  // Set up the callbacks
  platform.onWarn([&warnCalled, &warnMessage](const String &msg) {
    warnCalled = true;
    warnMessage = msg.c_str();
  });

  platform.onError([&errorCalled, &errorMessage](const String &msg) {
    errorCalled = true;
    errorMessage = msg.c_str();
  });

  platform.onDebug([&debugCalled, &debugMessage](const String &msg) {
    debugCalled = true;
    debugMessage = msg.c_str();
  });

  // Trigger the warning callback by registering a route with /api/ prefix
  platform.registerWebRoute(
      "/api/test", [](TestRequest &req, TestResponse &res) {}, {AuthType::NONE},
      WebModule::WM_GET);

  // Verify the warning callback was invoked
  TEST_ASSERT_TRUE(warnCalled);
  TEST_ASSERT_TRUE(warnMessage.find("/api/test") != std::string::npos);

  // We can't easily test error and debug in isolation, but at least we've
  // covered the callback setup and mechanism
}

// Test JSON conversion edge cases
void test_mock_web_platform_json_edge_cases() {
  MockWebPlatformProvider provider;
  MockWebPlatform &platform = provider.getMockPlatform();

  // Test empty JSON object
  TestResponse objResponse;
  platform.createJsonResponse(objResponse, [](JsonObject &obj) {
    // Empty - intentionally creating an empty object
  });
  TEST_ASSERT_EQUAL_STRING("{}", objResponse.getContent().c_str());
  TEST_ASSERT_EQUAL_STRING("application/json",
                           objResponse.getMimeType().c_str());

  // Test empty JSON array
  TestResponse arrResponse;
  platform.createJsonArrayResponse(arrResponse, [](JsonArray &arr) {
    // Empty - intentionally creating an empty array
  });
  TEST_ASSERT_EQUAL_STRING("[]", arrResponse.getContent().c_str());
  TEST_ASSERT_EQUAL_STRING("application/json",
                           arrResponse.getMimeType().c_str());

  // Test deeply nested structure (tests serialization)
  TestResponse nestedResponse;
  platform.createJsonResponse(nestedResponse, [](JsonObject &obj) {
    JsonObject level1 = obj.createNestedObject("level1");
    JsonObject level2 = level1.createNestedObject("level2");
    JsonObject level3 = level2.createNestedObject("level3");
    level3["deep"] = "value";
  });
  TEST_ASSERT_TRUE(nestedResponse.getContent().find("deep") !=
                   std::string::npos);
  TEST_ASSERT_TRUE(nestedResponse.getContent().find("value") !=
                   std::string::npos);
}

// Test MockWebPlatformProvider constructor and methods
void test_mock_web_platform_provider() {
  // Test constructor (line 157 in testing_platform_provider.h)
  MockWebPlatformProvider provider;

  // Test that getPlatform returns a valid reference
  IWebPlatform &platformRef = provider.getPlatform();
  TEST_ASSERT_NOT_NULL(&platformRef);

  // Test that getMockPlatform returns the correct type
  MockWebPlatform &mockPlatform = provider.getMockPlatform();
  TEST_ASSERT_NOT_NULL(&mockPlatform);

  // Verify we can use the platform through both interfaces
  mockPlatform.begin("TestName");
  TEST_ASSERT_EQUAL_STRING("TestName", platformRef.getDeviceName().c_str());
}

// Test parameter handling in mock request
void test_mock_web_platform_params() {
  MockWebPlatformProvider provider;

  // Create a TestRequest - the param methods are in this class, not in
  // MockWebPlatform
  TestRequest mockRequest;

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

// Test TestRequest constructor and methods (lines 80-85, 114-116, 136-138,
// 149-150)
void test_mock_web_request() {
  // Test constructor with default parameter (line 80-81)
  TestRequest defaultReq;
  TEST_ASSERT_EQUAL_STRING("/", defaultReq.getPath().c_str());

  // Test setParam and string conversion (lines 84-85)
  TestRequest req;
  req.setParam("test_param", "test_value");
  req.setParam("numeric", "123");

  // Test getParam with string conversion (lines 114-116)
  TEST_ASSERT_EQUAL_STRING("test_value", req.getParam("test_param").c_str());
  TEST_ASSERT_EQUAL_STRING("123", req.getParam("numeric").c_str());
  TEST_ASSERT_EQUAL_STRING("", req.getParam("nonexistent").c_str());

  // Test setJsonParam and getJsonParam (lines 107-108, 136-138)
  req.setJsonParam("json_param", "{\"key\":\"value\"}");
  TEST_ASSERT_EQUAL_STRING("{\"key\":\"value\"}",
                           req.getJsonParam("json_param").c_str());
  TEST_ASSERT_EQUAL_STRING("", req.getJsonParam("nonexistent_json").c_str());

  // Test getAllParams with string conversion (lines 149-150)
  auto params = req.getAllParams();
  TEST_ASSERT_EQUAL(
      2, params.size()); // Only counting regular params, not JSON params
  TEST_ASSERT_EQUAL_STRING("test_value", params["test_param"].c_str());
  TEST_ASSERT_EQUAL_STRING("123", params["numeric"].c_str());

  // Test headers
  req.setHeader("Content-Type", "application/json");
  req.setHeader("Authorization", "Bearer token");
  TEST_ASSERT_EQUAL_STRING("application/json",
                           req.getHeader("Content-Type").c_str());
  TEST_ASSERT_EQUAL_STRING("Bearer token",
                           req.getHeader("Authorization").c_str());
  TEST_ASSERT_EQUAL_STRING("", req.getHeader("nonexistent_header").c_str());

  // Test other setters
  req.setBody("{\"test\":\"body\"}");
  TEST_ASSERT_EQUAL_STRING("{\"test\":\"body\"}", req.getBody().c_str());

  req.setMethod(WebModule::WM_POST);
  TEST_ASSERT_EQUAL(WebModule::WM_POST, req.getMethod());

  req.setClientIp("192.168.1.1");
  TEST_ASSERT_EQUAL_STRING("192.168.1.1", req.getClientIp().c_str());
}

// Test TestResponse constructor and methods (lines 176-177, 185-186,
// 192-193, 197-198, 208-210)
void test_mock_web_response() {
  // Test constructor (lines 176-177)
  TestResponse res;
  TEST_ASSERT_EQUAL_STRING("", res.getContent().c_str());
  TEST_ASSERT_EQUAL_STRING("text/html", res.getMimeType().c_str());
  TEST_ASSERT_EQUAL(200, res.getStatus());

  // Test setContent (lines 185-186)
  res.setContent("Hello, World!", "text/plain");
  TEST_ASSERT_EQUAL_STRING("Hello, World!", res.getContent().c_str());
  TEST_ASSERT_EQUAL_STRING("text/plain", res.getMimeType().c_str());
  TEST_ASSERT_EQUAL(13, res.getContent().length());

  // Test setProgmemContent
  res.setProgmemContent("From PROGMEM", "text/html");
  TEST_ASSERT_TRUE(res.hasProgmemContent());
  TEST_ASSERT_EQUAL_STRING("From PROGMEM", res.getProgmemData());
  TEST_ASSERT_EQUAL_STRING("text/html", res.getMimeType().c_str());

  // Test setStatus
  res.setStatus(404);
  TEST_ASSERT_EQUAL(404, res.getStatus());

  // Test setHeader (lines 192-193)
  res.setHeader("Content-Encoding", "gzip");
  res.setHeader("Cache-Control", "no-cache");
  TEST_ASSERT_EQUAL_STRING("gzip", res.getHeader("Content-Encoding").c_str());
  TEST_ASSERT_EQUAL_STRING("no-cache", res.getHeader("Cache-Control").c_str());
  TEST_ASSERT_EQUAL_STRING("", res.getHeader("nonexistent_header").c_str());

  // Test redirect (lines 197-198)
  res.setRedirect("/new/location", 301);
  TEST_ASSERT_EQUAL(301, res.getStatus());
  TEST_ASSERT_EQUAL_STRING("/new/location", res.getHeader("Location").c_str());

  // Test defaults for redirect
  TestResponse res2;
  res2.setRedirect("/default/redirect");
  TEST_ASSERT_EQUAL(302, res2.getStatus()); // Default redirect code is 302
  TEST_ASSERT_EQUAL_STRING("/default/redirect",
                           res2.getHeader("Location").c_str());

  // Test mock-specific methods
  TEST_ASSERT_FALSE(res.isHeadersSent());
  TEST_ASSERT_FALSE(res.isResponseSent());

  res.markHeadersSent();
  TEST_ASSERT_TRUE(res.isHeadersSent());
  TEST_ASSERT_FALSE(res.isResponseSent());

  res.markResponseSent();
  TEST_ASSERT_TRUE(res.isResponseSent());
}

// Test Authentication handling in TestRequest (lines 96-97)
void test_mock_web_request_auth() {
  TestRequest req;

  // Test unauthenticated state (default)
  TEST_ASSERT_FALSE(req.getAuthContext().isAuthenticated);
  TEST_ASSERT_EQUAL_STRING("", req.getAuthContext().username.c_str());

  // Test setAuthContext with AuthContext object
  AuthContext authCtx;
  authCtx.isAuthenticated = true;
  authCtx.username = "testuser";
  authCtx.authenticatedVia = AuthType::SESSION;
  authCtx.sessionId = "test_session";

  req.setAuthContext(authCtx);
  TEST_ASSERT_TRUE(req.getAuthContext().isAuthenticated);
  TEST_ASSERT_EQUAL_STRING("testuser", req.getAuthContext().username.c_str());
  TEST_ASSERT_EQUAL(AuthType::SESSION, req.getAuthContext().authenticatedVia);
  TEST_ASSERT_EQUAL_STRING("test_session",
                           req.getAuthContext().sessionId.c_str());

  // Test setAuthContext with minimal AuthContext
  TestRequest req2;
  AuthContext minimalCtx;
  minimalCtx.isAuthenticated = true;
  req2.setAuthContext(minimalCtx);
  TEST_ASSERT_TRUE(req2.getAuthContext().isAuthenticated);
  TEST_ASSERT_EQUAL_STRING("", req2.getAuthContext().username.c_str());

  // Test setAuthContext with full auth context object
  TestRequest req3;
  AuthContext ctx;
  ctx.isAuthenticated = true;
  ctx.username = "apiuser";
  ctx.authenticatedVia = AuthType::TOKEN;
  ctx.sessionId = "api_token_123";

  req3.setAuthContext(ctx);
  TEST_ASSERT_TRUE(req3.getAuthContext().isAuthenticated);
  TEST_ASSERT_EQUAL_STRING("apiuser", req3.getAuthContext().username.c_str());
  TEST_ASSERT_EQUAL(AuthType::TOKEN, req3.getAuthContext().authenticatedVia);
  TEST_ASSERT_EQUAL_STRING("api_token_123",
                           req3.getAuthContext().sessionId.c_str());

  // Test route parameters (since we can't directly test getMatchedRoutePattern)
  TestRequest req4;
  req4.setParam("resource", "users");
  req4.setParam("id", "123");
  TEST_ASSERT_EQUAL_STRING("users", req4.getRouteParameter("resource").c_str());
  TEST_ASSERT_EQUAL_STRING("123", req4.getRouteParameter("id").c_str());
}

// Register all mock platform tests
void register_mock_web_platform_tests() {
  RUN_TEST(test_mock_web_platform_basics);
  RUN_TEST(test_mock_web_platform_routes);
  RUN_TEST(test_mock_web_platform_modules);
  RUN_TEST(test_mock_web_platform_json);
  RUN_TEST(test_mock_web_platform_json_edge_cases);
  RUN_TEST(test_mock_web_platform_callbacks);
  RUN_TEST(test_mock_web_platform_provider);
  RUN_TEST(test_mock_web_request);
  RUN_TEST(test_mock_web_response);
  RUN_TEST(test_mock_web_request_auth);
  RUN_TEST(test_mock_web_platform_params);
}
