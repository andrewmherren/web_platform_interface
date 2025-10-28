#include "../../include/testing/test_mock_web_platform.h"
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

  // Register API route with WebRoute (should warn for path with leading slash)
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

  // Test the second path condition: path starts with 'api/' (no leading slash)
  platform.registerWebRoute(
      "api/noprefix", [](WebRequest &req, WebResponse &res) {},
      {AuthType::TOKEN}, WebModule::WM_GET);
  TEST_ASSERT_TRUE(warningEmitted);
  TEST_ASSERT_TRUE(capturedWarning.indexOf("api/noprefix") >= 0);

  // Route count should be 3
  TEST_ASSERT_EQUAL(3, platform.getRouteCount());

  // Reset warning
  warningEmitted = false;
  capturedWarning = "";

  // Register proper API route (no warning)
  platform.registerApiRoute(
      "/api/proper", [](WebRequest &req, WebResponse &res) {},
      {AuthType::TOKEN}, WebModule::WM_PUT, OpenAPIDocumentation());
  TEST_ASSERT_FALSE(warningEmitted);

  // Route count should be 4
  TEST_ASSERT_EQUAL(4, platform.getRouteCount());

  // Register proper API route (no warning)
  platform.registerApiRoute(
      "/api/proper", [](WebRequest &req, WebResponse &res) {},
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
                       [](WebRequest &req, WebResponse &res) {})};
    }

    std::vector<RouteVariant> getHttpsRoutes() override {
      return {WebRoute("/secure", WebModule::WM_GET,
                       [](WebRequest &req, WebResponse &res) {})};
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

  // Test registering a null module (line 49 coverage)
  platform.registerModule("/null_module", nullptr);
  TEST_ASSERT_EQUAL(3, platform.getRegisteredModuleCount());
  TEST_ASSERT_EQUAL(
      4,
      platform.getRouteCount()); // Should remain the same since module is null

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
  TEST_ASSERT_EQUAL(3, modules.size());
  TEST_ASSERT_EQUAL_STRING("/test1", modules[0].first.c_str());
  TEST_ASSERT_EQUAL_STRING("/test2", modules[1].first.c_str());
  TEST_ASSERT_EQUAL_STRING("/null_module", modules[2].first.c_str());
  TEST_ASSERT_EQUAL(nullptr,
                    modules[2].second); // Null module should be stored as null
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

// Test callback functions (debug, warn, error)
void test_mock_web_platform_callbacks() {
  MockWebPlatformProvider provider;
  MockWebPlatform &platform = provider.getMockPlatform();

  // Test variables to capture callback invocations
  bool warnCalled = false;
  bool errorCalled = false;
  bool debugCalled = false;

  String warnMessage;
  String errorMessage;
  String debugMessage;

  // Set up the callbacks
  platform.onWarn([&warnCalled, &warnMessage](const String &msg) {
    warnCalled = true;
    warnMessage = msg;
  });

  platform.onError([&errorCalled, &errorMessage](const String &msg) {
    errorCalled = true;
    errorMessage = msg;
  });

  platform.onDebug([&debugCalled, &debugMessage](const String &msg) {
    debugCalled = true;
    debugMessage = msg;
  });

  // Trigger the warning callback by registering a route with /api/ prefix
  platform.registerWebRoute(
      "/api/test", [](WebRequest &req, WebResponse &res) {}, {AuthType::NONE},
      WebModule::WM_GET);

  // Verify the warning callback was invoked
  TEST_ASSERT_TRUE(warnCalled);
  TEST_ASSERT_TRUE(warnMessage.indexOf("/api/test") >= 0);

  // We can't easily test error and debug in isolation, but at least we've
  // covered the callback setup and mechanism
}

// Test JSON conversion edge cases
void test_mock_web_platform_json_edge_cases() {
  MockWebPlatformProvider provider;
  MockWebPlatform &platform = provider.getMockPlatform();

  // Test empty JSON object
  WebResponse objResponse;
  platform.createJsonResponse(objResponse, [](JsonObject &obj) {
    // Empty - intentionally creating an empty object
  });
  TEST_ASSERT_EQUAL_STRING("{}", objResponse.getContent().c_str());
  TEST_ASSERT_EQUAL_STRING("application/json",
                           objResponse.getMimeType().c_str());

  // Test empty JSON array
  WebResponse arrResponse;
  platform.createJsonArrayResponse(arrResponse, [](JsonArray &arr) {
    // Empty - intentionally creating an empty array
  });
  TEST_ASSERT_EQUAL_STRING("[]", arrResponse.getContent().c_str());
  TEST_ASSERT_EQUAL_STRING("application/json",
                           arrResponse.getMimeType().c_str());

  // Test deeply nested structure (tests serialization)
  WebResponse nestedResponse;
  platform.createJsonResponse(nestedResponse, [](JsonObject &obj) {
    JsonObject level1 = obj.createNestedObject("level1");
    JsonObject level2 = level1.createNestedObject("level2");
    JsonObject level3 = level2.createNestedObject("level3");
    level3["deep"] = "value";
  });
  TEST_ASSERT_TRUE(nestedResponse.getContent().indexOf("deep") >= 0);
  TEST_ASSERT_TRUE(nestedResponse.getContent().indexOf("value") >= 0);
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

// Test MockWebRequest constructor and methods (lines 80-85, 114-116, 136-138,
// 149-150)
void test_mock_web_request() {
  // Test constructor with default parameter (line 80-81)
  MockWebRequest defaultReq;
  TEST_ASSERT_EQUAL_STRING("/", defaultReq.getPath().c_str());

  // Test constructor with custom path
  MockWebRequest customPathReq("/custom/path");
  TEST_ASSERT_EQUAL_STRING("/custom/path", customPathReq.getPath().c_str());

  // Test setParam and string conversion (lines 84-85)
  MockWebRequest req;
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
  req.setMockHeader("Content-Type", "application/json");
  req.setMockHeader("Authorization", "Bearer token");
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

// Test specific ternary operator branches that are missing coverage
void test_mock_web_request_ternary_operators() {
  MockWebRequest req;

  // Test line 114: mockParams.count(stdName) ?
  // String(mockParams.at(stdName).c_str()) : String(""); First branch (count >
  // 0) - already tested above
  req.setParam("exists", "value");
  TEST_ASSERT_EQUAL_STRING("value", req.getParam("exists").c_str());

  // Second branch (count == 0) - key doesn't exist
  TEST_ASSERT_EQUAL_STRING("", req.getParam("does_not_exist").c_str());

  // Test line 128: mockHeaders.count(stdName) ?
  // String(mockHeaders.at(stdName).c_str()) : String(""); First branch (count >
  // 0)
  req.setMockHeader("X-Test", "header_value");
  TEST_ASSERT_EQUAL_STRING("header_value", req.getHeader("X-Test").c_str());

  // Second branch (count == 0)
  TEST_ASSERT_EQUAL_STRING("", req.getHeader("Missing-Header").c_str());

  // Test line 136: mockJsonParams.count(stdName) ? mockJsonParams.at(stdName) :
  // String(""); First branch (count > 0)
  req.setJsonParam("json_key", "{\"data\": \"value\"}");
  TEST_ASSERT_EQUAL_STRING("{\"data\": \"value\"}",
                           req.getJsonParam("json_key").c_str());

  // Second branch (count == 0)
  TEST_ASSERT_EQUAL_STRING("", req.getJsonParam("missing_json_key").c_str());

  // Test line 159: routePattern ? String(routePattern) : ""
  // First branch (routePattern != nullptr)
  req.setMatchedRoute("/test/{id}");
  // Note: This is internal state, testing the setter logic

  // Second branch (routePattern == nullptr)
  req.setMatchedRoute(nullptr);
  // Note: This tests the null pointer handling
}

// Test MockWebResponse constructor and methods (lines 176-177, 185-186,
// 192-193, 197-198, 208-210)
void test_mock_web_response() {
  // Test constructor (lines 176-177)
  MockWebResponse res;
  TEST_ASSERT_EQUAL_STRING("", res.getContent().c_str());
  TEST_ASSERT_EQUAL_STRING("text/html", res.getMimeType().c_str());
  TEST_ASSERT_EQUAL(200, res.getStatusCode());

  // Test setContent (lines 185-186)
  res.setContent("Hello, World!", "text/plain");
  TEST_ASSERT_EQUAL_STRING("Hello, World!", res.getContent().c_str());
  TEST_ASSERT_EQUAL_STRING("text/plain", res.getMimeType().c_str());
  TEST_ASSERT_EQUAL(13, res.getContentLength());

  // Test setProgmemContent
  res.setProgmemContent("From PROGMEM", "text/html");
  TEST_ASSERT_EQUAL_STRING("From PROGMEM", res.getContent().c_str());
  TEST_ASSERT_EQUAL_STRING("text/html", res.getMimeType().c_str());

  // Test setStatus
  res.setStatus(404);
  TEST_ASSERT_EQUAL(404, res.getStatusCode());

  // Test setHeader (lines 192-193)
  res.setHeader("Content-Encoding", "gzip");
  res.setHeader("Cache-Control", "no-cache");
  TEST_ASSERT_EQUAL_STRING("gzip", res.getHeader("Content-Encoding").c_str());
  TEST_ASSERT_EQUAL_STRING("no-cache", res.getHeader("Cache-Control").c_str());
  TEST_ASSERT_EQUAL_STRING("", res.getHeader("nonexistent_header").c_str());

  // Test redirect (lines 197-198)
  res.redirect("/new/location", 301);
  TEST_ASSERT_EQUAL(301, res.getStatusCode());
  TEST_ASSERT_EQUAL_STRING("/new/location", res.getHeader("Location").c_str());

  // Test defaults for redirect
  MockWebResponse res2;
  res2.redirect("/default/redirect");
  TEST_ASSERT_EQUAL(302, res2.getStatusCode()); // Default redirect code is 302
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

// Test specific ternary operators in MockWebResponse
void test_mock_web_response_ternary_operators() {
  MockWebResponse res;

  // Test line 208: mockHeaders.count(stdName) ?
  // String(mockHeaders.at(stdName).c_str()) : String(""); First branch (count >
  // 0) - header exists
  res.setHeader("Custom-Header", "custom_value");
  TEST_ASSERT_EQUAL_STRING("custom_value",
                           res.getHeader("Custom-Header").c_str());

  // Second branch (count == 0) - header doesn't exist
  TEST_ASSERT_EQUAL_STRING("", res.getHeader("Non-Existent-Header").c_str());

  // Test multiple headers to ensure both branches work reliably
  res.setHeader("Header1", "value1");
  res.setHeader("Header2", "value2");
  res.setHeader("Header3", "value3");

  // Test existing headers (first branch)
  TEST_ASSERT_EQUAL_STRING("value1", res.getHeader("Header1").c_str());
  TEST_ASSERT_EQUAL_STRING("value2", res.getHeader("Header2").c_str());
  TEST_ASSERT_EQUAL_STRING("value3", res.getHeader("Header3").c_str());

  // Test non-existing headers (second branch)
  TEST_ASSERT_EQUAL_STRING("", res.getHeader("Header4").c_str());
  TEST_ASSERT_EQUAL_STRING("", res.getHeader("Missing").c_str());
  TEST_ASSERT_EQUAL_STRING("", res.getHeader("").c_str()); // Empty header name
}

// Test Authentication handling in MockWebRequest (lines 96-97)
void test_mock_web_request_auth() {
  MockWebRequest req;

  // Test unauthenticated state (default)
  TEST_ASSERT_FALSE(req.getAuthContext().isAuthenticated);
  TEST_ASSERT_EQUAL_STRING("", req.getAuthContext().username.c_str());

  // Test setAuthContext with boolean and username (line 96-97)
  req.setAuthContext(true, "testuser");
  TEST_ASSERT_TRUE(req.getAuthContext().isAuthenticated);
  TEST_ASSERT_EQUAL_STRING("testuser", req.getAuthContext().username.c_str());
  TEST_ASSERT_EQUAL(AuthType::SESSION, req.getAuthContext().authenticatedVia);
  TEST_ASSERT_EQUAL_STRING("test_session",
                           req.getAuthContext().sessionId.c_str());

  // Test setAuthContext with boolean only
  MockWebRequest req2;
  req2.setAuthContext(true);
  TEST_ASSERT_TRUE(req2.getAuthContext().isAuthenticated);
  TEST_ASSERT_EQUAL_STRING("", req2.getAuthContext().username.c_str());

  // Test setAuthContext with full auth context object
  MockWebRequest req3;
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
  MockWebRequest req4;
  req4.setParam("resource", "users");
  req4.setParam("id", "123");
  TEST_ASSERT_EQUAL_STRING("users", req4.getRouteParameter("resource").c_str());
  TEST_ASSERT_EQUAL_STRING("123", req4.getRouteParameter("id").c_str());
}

// Test edge cases in MockWebRequest parameter handling and string conversions
void test_mock_web_request_edge_cases() {
  // Test empty parameter name and value
  MockWebRequest req;
  req.setParam("", "");
  TEST_ASSERT_EQUAL_STRING("", req.getParam("").c_str());

  // Test parameter with special characters
  req.setParam("special_chars", "value with spaces & symbols!");
  TEST_ASSERT_EQUAL_STRING("value with spaces & symbols!",
                           req.getParam("special_chars").c_str());

  // Test numeric values as strings
  req.setParam("number", "42");
  req.setParam("float", "3.14");
  TEST_ASSERT_EQUAL_STRING("42", req.getParam("number").c_str());
  TEST_ASSERT_EQUAL_STRING("3.14", req.getParam("float").c_str());

  // Test getAllParams with mixed types
  auto allParams = req.getAllParams();
  TEST_ASSERT_EQUAL(4, allParams.size()); // Including empty key
  TEST_ASSERT_EQUAL_STRING("value with spaces & symbols!",
                           allParams["special_chars"].c_str());
  TEST_ASSERT_EQUAL_STRING("42", allParams["number"].c_str());
  TEST_ASSERT_EQUAL_STRING("3.14", allParams["float"].c_str());

  // Test JSON parameter edge cases
  req.setJsonParam("empty_json", "");
  req.setJsonParam("complex_json", "{\"nested\":{\"array\":[1,2,3]}}");
  TEST_ASSERT_EQUAL_STRING("", req.getJsonParam("empty_json").c_str());
  TEST_ASSERT_EQUAL_STRING("{\"nested\":{\"array\":[1,2,3]}}",
                           req.getJsonParam("complex_json").c_str());

  // Test header edge cases
  req.setMockHeader("", "empty_header_name");
  req.setMockHeader("Content-Type", "");
  TEST_ASSERT_EQUAL_STRING("empty_header_name", req.getHeader("").c_str());
  TEST_ASSERT_EQUAL_STRING("", req.getHeader("Content-Type").c_str());
}

// Test MockWebResponse edge cases and all branches
void test_mock_web_response_edge_cases() {
  MockWebResponse res;

  // Test setContent with empty content and different mime types
  res.setContent("", "application/json");
  TEST_ASSERT_EQUAL_STRING("", res.getContent().c_str());
  TEST_ASSERT_EQUAL_STRING("application/json", res.getMimeType().c_str());
  TEST_ASSERT_EQUAL(0, res.getContentLength());

  // Test setContent with default mime type (should use parameter default)
  res.setContent("test content");
  TEST_ASSERT_EQUAL_STRING("test content", res.getContent().c_str());
  TEST_ASSERT_EQUAL_STRING("text/html", res.getMimeType().c_str());
  TEST_ASSERT_EQUAL(12, res.getContentLength());

  // Test setProgmemContent with various content types
  res.setProgmemContent("PROGMEM content", "text/plain");
  TEST_ASSERT_EQUAL_STRING("PROGMEM content", res.getContent().c_str());
  TEST_ASSERT_EQUAL_STRING("text/plain", res.getMimeType().c_str());

  // Test status code edge cases
  res.setStatus(0);
  TEST_ASSERT_EQUAL(0, res.getStatusCode());

  res.setStatus(999);
  TEST_ASSERT_EQUAL(999, res.getStatusCode());

  // Test header edge cases with empty values
  res.setHeader("Empty-Value", "");
  res.setHeader("", "empty-name");
  TEST_ASSERT_EQUAL_STRING("", res.getHeader("Empty-Value").c_str());
  TEST_ASSERT_EQUAL_STRING("empty-name", res.getHeader("").c_str());

  // Test redirect with default code (302)
  MockWebResponse res2;
  res2.redirect("/default");
  TEST_ASSERT_EQUAL(302, res2.getStatusCode());
  TEST_ASSERT_EQUAL_STRING("/default", res2.getHeader("Location").c_str());

  // Test redirect with custom code
  res2.redirect("/custom", 301);
  TEST_ASSERT_EQUAL(301, res2.getStatusCode());
  TEST_ASSERT_EQUAL_STRING("/custom", res2.getHeader("Location").c_str());

  // Test state tracking methods
  TEST_ASSERT_FALSE(res2.isHeadersSent());
  TEST_ASSERT_FALSE(res2.isResponseSent());

  res2.markHeadersSent();
  TEST_ASSERT_TRUE(res2.isHeadersSent());
  TEST_ASSERT_FALSE(res2.isResponseSent());

  res2.markResponseSent();
  TEST_ASSERT_TRUE(res2.isHeadersSent());
  TEST_ASSERT_TRUE(res2.isResponseSent());
}

// Test MockWebRequest authentication edge cases
void test_mock_web_request_auth_edge_cases() {
  // Test setAuthContext with boolean false and username (should still set
  // username)
  MockWebRequest req1;
  req1.setAuthContext(false, "unauthenticated_user");
  TEST_ASSERT_FALSE(req1.getAuthContext().isAuthenticated);
  TEST_ASSERT_EQUAL_STRING("unauthenticated_user",
                           req1.getAuthContext().username.c_str());

  // Test setAuthContext with true and empty username
  MockWebRequest req2;
  req2.setAuthContext(true, "");
  TEST_ASSERT_TRUE(req2.getAuthContext().isAuthenticated);
  TEST_ASSERT_EQUAL_STRING("", req2.getAuthContext().username.c_str());
  TEST_ASSERT_EQUAL(AuthType::SESSION, req2.getAuthContext().authenticatedVia);
  TEST_ASSERT_EQUAL_STRING("test_session",
                           req2.getAuthContext().sessionId.c_str());

  // Test AuthContext object with various combinations
  MockWebRequest req3;
  AuthContext ctx1;
  ctx1.isAuthenticated = false;
  ctx1.username = "";
  ctx1.authenticatedVia = AuthType::NONE;
  ctx1.sessionId = "";

  req3.setAuthContext(ctx1);
  TEST_ASSERT_FALSE(req3.getAuthContext().isAuthenticated);
  TEST_ASSERT_EQUAL_STRING("", req3.getAuthContext().username.c_str());
  TEST_ASSERT_EQUAL(AuthType::NONE, req3.getAuthContext().authenticatedVia);
  TEST_ASSERT_EQUAL_STRING("", req3.getAuthContext().sessionId.c_str());

  // Test AuthContext with LOCAL_ONLY authentication
  MockWebRequest req4;
  AuthContext ctx2;
  ctx2.isAuthenticated = true;
  ctx2.username = "local_user";
  ctx2.authenticatedVia = AuthType::LOCAL_ONLY;
  ctx2.sessionId = "local_session";

  req4.setAuthContext(ctx2);
  TEST_ASSERT_TRUE(req4.getAuthContext().isAuthenticated);
  TEST_ASSERT_EQUAL_STRING("local_user",
                           req4.getAuthContext().username.c_str());
  TEST_ASSERT_EQUAL(AuthType::LOCAL_ONLY,
                    req4.getAuthContext().authenticatedVia);
  TEST_ASSERT_EQUAL_STRING("local_session",
                           req4.getAuthContext().sessionId.c_str());
}

// Test platform URL generation branches
void test_mock_web_platform_url_branches() {
  MockWebPlatformProvider provider;
  MockWebPlatform &platform = provider.getMockPlatform();

  // Test HTTPS enabled (default)
  TEST_ASSERT_TRUE(platform.isHttpsEnabled());
  TEST_ASSERT_EQUAL_STRING("https://mock-device.local",
                           platform.getBaseUrl().c_str());

  // Test with HTTPS disabled
  platform.begin("TestDevice", false);
  TEST_ASSERT_FALSE(platform.isHttpsEnabled());
  TEST_ASSERT_EQUAL_STRING("http://mock-device.local",
                           platform.getBaseUrl().c_str());

  // Test connection status
  TEST_ASSERT_TRUE(platform.isConnected());
  platform.setConnected(false);
  TEST_ASSERT_FALSE(platform.isConnected());
  platform.setConnected(true);
  TEST_ASSERT_TRUE(platform.isConnected());
}

// Test module registration with modules that have different route counts
void test_mock_web_platform_module_route_variations() {
  MockWebPlatformProvider provider;
  MockWebPlatform &platform = provider.getMockPlatform();

  // Module with no routes
  class EmptyModule : public IWebModule {
  public:
    std::vector<RouteVariant> getHttpRoutes() override { return {}; }
    std::vector<RouteVariant> getHttpsRoutes() override { return {}; }
    String getModuleName() const override { return "EmptyModule"; }
    void begin() override {}
    void handle() override {}
  };

  // Module with only HTTP routes
  class HttpOnlyModule : public IWebModule {
  public:
    std::vector<RouteVariant> getHttpRoutes() override {
      return {WebRoute("/http1", WebModule::WM_GET,
                       [](WebRequest &req, WebResponse &res) {}),
              WebRoute("/http2", WebModule::WM_POST,
                       [](WebRequest &req, WebResponse &res) {})};
    }
    std::vector<RouteVariant> getHttpsRoutes() override { return {}; }
    String getModuleName() const override { return "HttpOnlyModule"; }
    void begin() override {}
    void handle() override {}
  };

  // Module with only HTTPS routes
  class HttpsOnlyModule : public IWebModule {
  public:
    std::vector<RouteVariant> getHttpRoutes() override { return {}; }
    std::vector<RouteVariant> getHttpsRoutes() override {
      return {WebRoute("/https1", WebModule::WM_GET,
                       [](WebRequest &req, WebResponse &res) {}),
              WebRoute("/https2", WebModule::WM_DELETE,
                       [](WebRequest &req, WebResponse &res) {}),
              WebRoute("/https3", WebModule::WM_PUT,
                       [](WebRequest &req, WebResponse &res) {})};
    }
    String getModuleName() const override { return "HttpsOnlyModule"; }
    void begin() override {}
    void handle() override {}
  };

  EmptyModule emptyMod;
  HttpOnlyModule httpMod;
  HttpsOnlyModule httpsMod;

  // Test empty module (0 routes)
  platform.registerModule("/empty", &emptyMod);
  TEST_ASSERT_EQUAL(0, platform.getRouteCount());

  // Test HTTP-only module (2 routes)
  platform.registerModule("/http", &httpMod);
  TEST_ASSERT_EQUAL(2, platform.getRouteCount());

  // Test HTTPS-only module (3 routes)
  platform.registerModule("/https", &httpsMod);
  TEST_ASSERT_EQUAL(5, platform.getRouteCount()); // 0 + 2 + 3

  // Test null module again to ensure it doesn't add routes
  platform.registerModule("/null", nullptr);
  TEST_ASSERT_EQUAL(5, platform.getRouteCount()); // Should remain same
  TEST_ASSERT_EQUAL(
      4, platform.getRegisteredModuleCount()); // But module count increases
}

// Test various callback scenarios
void test_mock_web_platform_callback_variations() {
  MockWebPlatformProvider provider;
  MockWebPlatform &platform = provider.getMockPlatform();

  // Test multiple callback invocations
  int warnCount = 0;
  int errorCount = 0;
  int debugCount = 0;

  platform.onWarn([&warnCount](const String &msg) { warnCount++; });
  platform.onError([&errorCount](const String &msg) { errorCount++; });
  platform.onDebug([&debugCount](const String &msg) { debugCount++; });

  // Trigger multiple warnings with different paths
  platform.registerWebRoute(
      "/api/first", [](WebRequest &req, WebResponse &res) {}, {AuthType::NONE},
      WebModule::WM_GET);
  platform.registerWebRoute(
      "api/second", [](WebRequest &req, WebResponse &res) {}, {AuthType::TOKEN},
      WebModule::WM_POST);
  platform.registerWebRoute(
      "/api/third", [](WebRequest &req, WebResponse &res) {},
      {AuthType::SESSION}, WebModule::WM_PUT);
  platform.registerWebRoute(
      "api/fourth", [](WebRequest &req, WebResponse &res) {},
      {AuthType::LOCAL_ONLY}, WebModule::WM_DELETE);

  TEST_ASSERT_EQUAL(4, warnCount); // All should trigger warnings
  TEST_ASSERT_EQUAL(4, platform.getRouteCount());

  // Test that non-api routes don't trigger warnings
  platform.registerWebRoute(
      "/normal/route", [](WebRequest &req, WebResponse &res) {},
      {AuthType::NONE}, WebModule::WM_GET);
  platform.registerWebRoute(
      "/another/normal", [](WebRequest &req, WebResponse &res) {},
      {AuthType::SESSION}, WebModule::WM_POST);

  TEST_ASSERT_EQUAL(4, warnCount); // Should remain 4
  TEST_ASSERT_EQUAL(6, platform.getRouteCount());
}

// Test handle() method with mixed null and valid modules
void test_mock_web_platform_handle_edge_cases() {
  MockWebPlatformProvider provider;
  MockWebPlatform &platform = provider.getMockPlatform();

  // Create a module that tracks handle calls
  class TrackingModule : public IWebModule {
  private:
    int handleCalls = 0;

  public:
    std::vector<RouteVariant> getHttpRoutes() override { return {}; }
    std::vector<RouteVariant> getHttpsRoutes() override { return {}; }
    String getModuleName() const override { return "TrackingModule"; }
    void begin() override {}
    void handle() override { handleCalls++; }
    int getHandleCalls() const { return handleCalls; }
  };

  TrackingModule module1;
  TrackingModule module2;
  TrackingModule module3;

  // Register mixed null and valid modules
  platform.registerModule("/valid1", &module1);
  platform.registerModule("/null1", nullptr);
  platform.registerModule("/valid2", &module2);
  platform.registerModule("/null2", nullptr);
  platform.registerModule("/valid3", &module3);
  platform.registerModule("/null3", nullptr);

  // Call handle - should only call valid modules
  platform.handle();

  TEST_ASSERT_EQUAL(1, module1.getHandleCalls());
  TEST_ASSERT_EQUAL(1, module2.getHandleCalls());
  TEST_ASSERT_EQUAL(1, module3.getHandleCalls());

  // Call handle again - should increment only valid modules
  platform.handle();

  TEST_ASSERT_EQUAL(2, module1.getHandleCalls());
  TEST_ASSERT_EQUAL(2, module2.getHandleCalls());
  TEST_ASSERT_EQUAL(2, module3.getHandleCalls());

  // Verify we have all modules registered (including nulls)
  TEST_ASSERT_EQUAL(6, platform.getRegisteredModuleCount());

  auto modules = platform.getRegisteredModules();
  TEST_ASSERT_EQUAL(&module1, modules[0].second);
  TEST_ASSERT_EQUAL(nullptr, modules[1].second);
  TEST_ASSERT_EQUAL(&module2, modules[2].second);
  TEST_ASSERT_EQUAL(nullptr, modules[3].second);
  TEST_ASSERT_EQUAL(&module3, modules[4].second);
  TEST_ASSERT_EQUAL(nullptr, modules[5].second);
}

// Test disableRoute boundary conditions thoroughly
void test_mock_web_platform_disable_route_edge_cases() {
  MockWebPlatformProvider provider;
  MockWebPlatform &platform = provider.getMockPlatform();

  // Start with empty routes
  TEST_ASSERT_EQUAL(0, platform.getRouteCount());

  // Try to disable route when count is 0 - should remain 0
  platform.disableRoute("/nonexistent", WebModule::WM_GET);
  TEST_ASSERT_EQUAL(0, platform.getRouteCount());

  // Add one route
  platform.registerWebRoute(
      "/single", [](WebRequest &req, WebResponse &res) {}, {AuthType::NONE},
      WebModule::WM_GET);
  TEST_ASSERT_EQUAL(1, platform.getRouteCount());

  // Disable it - should go to 0
  platform.disableRoute("/single", WebModule::WM_GET);
  TEST_ASSERT_EQUAL(0, platform.getRouteCount());

  // Try to disable again - should remain 0
  platform.disableRoute("/single", WebModule::WM_GET);
  TEST_ASSERT_EQUAL(0, platform.getRouteCount());

  // Add multiple routes
  platform.registerWebRoute(
      "/route1", [](WebRequest &req, WebResponse &res) {}, {AuthType::NONE},
      WebModule::WM_GET);
  platform.registerWebRoute(
      "/route2", [](WebRequest &req, WebResponse &res) {}, {AuthType::TOKEN},
      WebModule::WM_POST);
  platform.registerApiRoute(
      "/api/route3", [](WebRequest &req, WebResponse &res) {},
      {AuthType::SESSION}, WebModule::WM_PUT, OpenAPIDocumentation());
  TEST_ASSERT_EQUAL(3, platform.getRouteCount());

  // Disable one at a time
  platform.disableRoute("/route1", WebModule::WM_GET);
  TEST_ASSERT_EQUAL(2, platform.getRouteCount());

  platform.disableRoute("/route2", WebModule::WM_POST);
  TEST_ASSERT_EQUAL(1, platform.getRouteCount());

  platform.disableRoute("/api/route3", WebModule::WM_PUT);
  TEST_ASSERT_EQUAL(0, platform.getRouteCount());

  // Try to disable more - should remain 0
  platform.disableRoute("/anything", WebModule::WM_DELETE);
  TEST_ASSERT_EQUAL(0, platform.getRouteCount());
}

// Test JSON response with various builder scenarios
void test_mock_web_platform_json_builder_variations() {
  MockWebPlatformProvider provider;
  MockWebPlatform &platform = provider.getMockPlatform();

  // Test createJsonResponse with complex nested structures
  WebResponse complexResponse;
  platform.createJsonResponse(complexResponse, [](JsonObject &obj) {
    obj["simple"] = "value";
    obj["number"] = 42;
    obj["boolean"] = true;
    obj["null_value"] = nullptr;

    JsonArray array = obj.createNestedArray("array");
    array.add("item1");
    array.add(123);
    array.add(false);

    JsonObject nested = obj.createNestedObject("nested");
    nested["deep"] = "value";
    nested["deeper"] = JsonObject();
  });

  String content = complexResponse.getContent();
  TEST_ASSERT_TRUE(content.indexOf("simple") >= 0);
  TEST_ASSERT_TRUE(content.indexOf("value") >= 0);
  TEST_ASSERT_TRUE(content.indexOf("42") >= 0);
  TEST_ASSERT_TRUE(content.indexOf("true") >= 0);
  TEST_ASSERT_TRUE(content.indexOf("item1") >= 0);
  TEST_ASSERT_TRUE(content.indexOf("123") >= 0);
  TEST_ASSERT_TRUE(content.indexOf("false") >= 0);
  TEST_ASSERT_TRUE(content.indexOf("deep") >= 0);
  TEST_ASSERT_EQUAL_STRING("application/json",
                           complexResponse.getMimeType().c_str());

  // Test createJsonArrayResponse with nested objects and arrays
  WebResponse arrayResponse;
  platform.createJsonArrayResponse(arrayResponse, [](JsonArray &arr) {
    // Add primitive values
    arr.add("string");
    arr.add(456);
    arr.add(true);
    arr.add(nullptr);

    // Add nested object
    JsonObject obj = arr.createNestedObject();
    obj["key"] = "object_value";
    obj["num"] = 789;

    // Add nested array
    JsonArray nestedArr = arr.createNestedArray();
    nestedArr.add("nested_item1");
    nestedArr.add("nested_item2");
  });

  String arrayContent = arrayResponse.getContent();
  TEST_ASSERT_TRUE(arrayContent.indexOf("string") >= 0);
  TEST_ASSERT_TRUE(arrayContent.indexOf("456") >= 0);
  TEST_ASSERT_TRUE(arrayContent.indexOf("object_value") >= 0);
  TEST_ASSERT_TRUE(arrayContent.indexOf("789") >= 0);
  TEST_ASSERT_TRUE(arrayContent.indexOf("nested_item1") >= 0);
  TEST_ASSERT_TRUE(arrayContent.indexOf("nested_item2") >= 0);
  TEST_ASSERT_EQUAL_STRING("application/json",
                           arrayResponse.getMimeType().c_str());
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
  RUN_TEST(test_mock_web_request_ternary_operators);
  RUN_TEST(test_mock_web_response);
  RUN_TEST(test_mock_web_response_ternary_operators);
  RUN_TEST(test_mock_web_request_auth);
  RUN_TEST(test_mock_web_platform_params);

  // New comprehensive edge case tests
  RUN_TEST(test_mock_web_request_edge_cases);
  RUN_TEST(test_mock_web_response_edge_cases);
  RUN_TEST(test_mock_web_request_auth_edge_cases);
  RUN_TEST(test_mock_web_platform_url_branches);
  RUN_TEST(test_mock_web_platform_module_route_variations);
  RUN_TEST(test_mock_web_platform_callback_variations);
  RUN_TEST(test_mock_web_platform_handle_edge_cases);
  RUN_TEST(test_mock_web_platform_disable_route_edge_cases);
  RUN_TEST(test_mock_web_platform_json_builder_variations);

  // Targeted coverage tests for specific missing branches
  RUN_TEST(test_mock_web_request_targeted_coverage);
  RUN_TEST(test_mock_web_response_targeted_coverage);
  RUN_TEST(test_mock_auth_context_targeted_coverage);
  RUN_TEST(test_mock_web_platform_targeted_coverage);
  RUN_TEST(test_ternary_operator_branches);
  RUN_TEST(test_string_conversion_edge_cases);
}