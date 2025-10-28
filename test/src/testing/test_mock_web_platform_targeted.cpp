#include "../../include/testing/test_mock_web_platform.h"
#include "interface/openapi_types.h"
#include <ArduinoFake.h>
#include <testing/testing_platform_provider.h>
#include <unity.h>
#include <web_platform_interface.h>

// Targeted tests for very specific missing branch coverage
// Based on the coverage report lines: 80, 84, 103, 107, 114, 115, 116, 128,
// 129, 130, 149, 159, 176, 185, 192, 197, 208, 209, 210

// Test line 80: Constructor default parameter branch coverage
void test_mock_web_request_targeted_coverage() {
  // Line 80: MockWebRequest(const String &path = "/")
  // Test both branches of the default parameter

  // Branch 1: Explicit path provided
  MockWebRequest req1("/explicit/path");
  TEST_ASSERT_EQUAL_STRING("/explicit/path", req1.getPath().c_str());

  // Branch 2: Default parameter used (empty constructor call)
  MockWebRequest req2; // This uses the default "/"
  TEST_ASSERT_EQUAL_STRING("/", req2.getPath().c_str());

  // Branch 3: Explicitly passing default value
  MockWebRequest req3("/");
  TEST_ASSERT_EQUAL_STRING("/", req3.getPath().c_str());

  // Branch 4: Empty string (different from default)
  MockWebRequest req4("");
  TEST_ASSERT_EQUAL_STRING("", req4.getPath().c_str());

  // Line 84-85: setParam string conversions
  // Test various edge cases for string conversion paths
  MockWebRequest req;

  // Test with empty key and value (edge case)
  req.setParam("", "");
  TEST_ASSERT_EQUAL_STRING("", req.getParam("").c_str());

  // Test with special characters that might affect string conversion
  req.setParam("special", "!@#$%^&*()");
  TEST_ASSERT_EQUAL_STRING("!@#$%^&*()", req.getParam("special").c_str());

  // Test with very long strings
  String longKey = "very_long_parameter_key_that_exceeds_normal_length";
  String longValue = "very_long_parameter_value_that_also_exceeds_normal_"
                     "length_and_contains_various_characters_123456789";
  req.setParam(longKey, longValue);
  TEST_ASSERT_EQUAL_STRING(longValue.c_str(), req.getParam(longKey).c_str());

  // Test numeric strings (different conversion paths)
  req.setParam("zero", "0");
  req.setParam("negative", "-123");
  req.setParam("float", "123.456");
  req.setParam("scientific", "1.23e10");

  TEST_ASSERT_EQUAL_STRING("0", req.getParam("zero").c_str());
  TEST_ASSERT_EQUAL_STRING("-123", req.getParam("negative").c_str());
  TEST_ASSERT_EQUAL_STRING("123.456", req.getParam("float").c_str());
  TEST_ASSERT_EQUAL_STRING("1.23e10", req.getParam("scientific").c_str());

  // Line 103: setMockHeader string conversions
  req.setMockHeader("", "empty_key_header");
  req.setMockHeader("empty_value", "");
  req.setMockHeader("unicode", "测试");

  TEST_ASSERT_EQUAL_STRING("empty_key_header", req.getHeader("").c_str());
  TEST_ASSERT_EQUAL_STRING("", req.getHeader("empty_value").c_str());
  TEST_ASSERT_EQUAL_STRING("测试", req.getHeader("unicode").c_str());

  // Line 107: setJsonParam string storage
  req.setJsonParam("", "{}");
  req.setJsonParam(
      "complex",
      "{\"nested\":{\"array\":[1,2,3],\"null\":null,\"bool\":true}}");
  req.setJsonParam("malformed", "{invalid json}");

  TEST_ASSERT_EQUAL_STRING("{}", req.getJsonParam("").c_str());
  TEST_ASSERT_EQUAL_STRING(
      "{\"nested\":{\"array\":[1,2,3],\"null\":null,\"bool\":true}}",
      req.getJsonParam("complex").c_str());
  TEST_ASSERT_EQUAL_STRING("{invalid json}",
                           req.getJsonParam("malformed").c_str());
}

// Test targeted MockWebResponse coverage
void test_mock_web_response_targeted_coverage() {
  // Line 176: Constructor initialization branches
  MockWebResponse res1;
  TEST_ASSERT_EQUAL_STRING("", res1.getContent().c_str());
  TEST_ASSERT_EQUAL_STRING("text/html", res1.getMimeType().c_str());
  TEST_ASSERT_EQUAL(200, res1.getStatusCode());

  // Line 185: setContent parameter branches
  // Branch 1: Both parameters provided
  MockWebResponse res2;
  res2.setContent("content", "application/json");
  TEST_ASSERT_EQUAL_STRING("content", res2.getContent().c_str());
  TEST_ASSERT_EQUAL_STRING("application/json", res2.getMimeType().c_str());

  // Branch 2: Default content type (testing default parameter)
  MockWebResponse res3;
  res3.setContent("html content"); // Should use default "text/html"
  TEST_ASSERT_EQUAL_STRING("html content", res3.getContent().c_str());
  TEST_ASSERT_EQUAL_STRING("text/html", res3.getMimeType().c_str());

  // Branch 3: Empty content with custom type
  MockWebResponse res4;
  res4.setContent("", "text/plain");
  TEST_ASSERT_EQUAL_STRING("", res4.getContent().c_str());
  TEST_ASSERT_EQUAL_STRING("text/plain", res4.getMimeType().c_str());
  TEST_ASSERT_EQUAL(0, res4.getContentLength());

  // Line 192-193: setHeader string conversion branches
  MockWebResponse res5;
  res5.setHeader("", "empty_name");
  res5.setHeader("empty_value", "");
  res5.setHeader("normal", "value");
  res5.setHeader("special_chars", "value!@#$%^&*()");
  res5.setHeader("unicode_header", "测试值");

  TEST_ASSERT_EQUAL_STRING("empty_name", res5.getHeader("").c_str());
  TEST_ASSERT_EQUAL_STRING("", res5.getHeader("empty_value").c_str());
  TEST_ASSERT_EQUAL_STRING("value", res5.getHeader("normal").c_str());
  TEST_ASSERT_EQUAL_STRING("value!@#$%^&*()",
                           res5.getHeader("special_chars").c_str());
  TEST_ASSERT_EQUAL_STRING("测试值", res5.getHeader("unicode_header").c_str());

  // Line 197-198: redirect parameter branches
  // Branch 1: Custom code provided
  MockWebResponse res6;
  res6.redirect("/custom", 301);
  TEST_ASSERT_EQUAL(301, res6.getStatusCode());
  TEST_ASSERT_EQUAL_STRING("/custom", res6.getHeader("Location").c_str());

  // Branch 2: Default code (302)
  MockWebResponse res7;
  res7.redirect("/default");
  TEST_ASSERT_EQUAL(302, res7.getStatusCode());
  TEST_ASSERT_EQUAL_STRING("/default", res7.getHeader("Location").c_str());

  // Branch 3: Edge case status codes
  MockWebResponse res8;
  res8.redirect("/permanent", 308);
  TEST_ASSERT_EQUAL(308, res8.getStatusCode());
  TEST_ASSERT_EQUAL_STRING("/permanent", res8.getHeader("Location").c_str());

  // Branch 4: Empty redirect URL
  MockWebResponse res9;
  res9.redirect("", 303);
  TEST_ASSERT_EQUAL(303, res9.getStatusCode());
  TEST_ASSERT_EQUAL_STRING("", res9.getHeader("Location").c_str());
}

// Test authentication context edge cases
void test_mock_auth_context_targeted_coverage() {
  MockWebRequest req;

  // Line 96: setAuthContext branches (the one that was fixed)
  // But let's test all the conditional branches more thoroughly

  // Branch 1: authenticated=false, no username
  req.setAuthContext(false);
  TEST_ASSERT_FALSE(req.getAuthContext().isAuthenticated);
  TEST_ASSERT_EQUAL_STRING("", req.getAuthContext().username.c_str());

  // Branch 2: authenticated=false, with username
  req.setAuthContext(false, "should_not_auth");
  TEST_ASSERT_FALSE(req.getAuthContext().isAuthenticated);
  TEST_ASSERT_EQUAL_STRING("should_not_auth",
                           req.getAuthContext().username.c_str());

  // Branch 3: authenticated=true, no username (triggers the if branch)
  req.setAuthContext(true);
  TEST_ASSERT_TRUE(req.getAuthContext().isAuthenticated);
  TEST_ASSERT_EQUAL_STRING("", req.getAuthContext().username.c_str());
  TEST_ASSERT_EQUAL(AuthType::SESSION, req.getAuthContext().authenticatedVia);
  TEST_ASSERT_EQUAL_STRING("test_session",
                           req.getAuthContext().sessionId.c_str());

  // Branch 4: authenticated=true, with username (triggers the if branch)
  req.setAuthContext(true, "test_user");
  TEST_ASSERT_TRUE(req.getAuthContext().isAuthenticated);
  TEST_ASSERT_EQUAL_STRING("test_user", req.getAuthContext().username.c_str());
  TEST_ASSERT_EQUAL(AuthType::SESSION, req.getAuthContext().authenticatedVia);
  TEST_ASSERT_EQUAL_STRING("test_session",
                           req.getAuthContext().sessionId.c_str());

  // Test AuthContext object setting with various combinations
  AuthContext ctx1;
  ctx1.isAuthenticated = false;
  ctx1.username = "ctx_user";
  ctx1.authenticatedVia = AuthType::NONE;
  ctx1.sessionId = "";
  req.setAuthContext(ctx1);

  TEST_ASSERT_FALSE(req.getAuthContext().isAuthenticated);
  TEST_ASSERT_EQUAL_STRING("ctx_user", req.getAuthContext().username.c_str());
  TEST_ASSERT_EQUAL(AuthType::NONE, req.getAuthContext().authenticatedVia);
  TEST_ASSERT_EQUAL_STRING("", req.getAuthContext().sessionId.c_str());

  // Test with TOKEN auth type
  AuthContext ctx2;
  ctx2.isAuthenticated = true;
  ctx2.username = "api_user";
  ctx2.authenticatedVia = AuthType::TOKEN;
  ctx2.sessionId = "api_token_123";
  req.setAuthContext(ctx2);

  TEST_ASSERT_TRUE(req.getAuthContext().isAuthenticated);
  TEST_ASSERT_EQUAL_STRING("api_user", req.getAuthContext().username.c_str());
  TEST_ASSERT_EQUAL(AuthType::TOKEN, req.getAuthContext().authenticatedVia);
  TEST_ASSERT_EQUAL_STRING("api_token_123",
                           req.getAuthContext().sessionId.c_str());

  // Test with LOCAL_ONLY auth type
  AuthContext ctx3;
  ctx3.isAuthenticated = true;
  ctx3.username = "";
  ctx3.authenticatedVia = AuthType::LOCAL_ONLY;
  ctx3.sessionId = "local_session";
  req.setAuthContext(ctx3);

  TEST_ASSERT_TRUE(req.getAuthContext().isAuthenticated);
  TEST_ASSERT_EQUAL_STRING("", req.getAuthContext().username.c_str());
  TEST_ASSERT_EQUAL(AuthType::LOCAL_ONLY,
                    req.getAuthContext().authenticatedVia);
  TEST_ASSERT_EQUAL_STRING("local_session",
                           req.getAuthContext().sessionId.c_str());
}

// Test MockWebPlatform targeted coverage for specific lines
void test_mock_web_platform_targeted_coverage() {
  MockWebPlatformProvider provider;
  MockWebPlatform &platform = provider.getMockPlatform();

  // Test begin() method overloads
  // Overload 1: name only
  platform.begin("Device1");
  TEST_ASSERT_EQUAL_STRING("Device1", platform.getDeviceName().c_str());
  TEST_ASSERT_TRUE(platform.isHttpsEnabled()); // Default should be true

  // Overload 2: name and httpsOnly
  platform.begin("Device2", false);
  TEST_ASSERT_EQUAL_STRING("Device2", platform.getDeviceName().c_str());
  TEST_ASSERT_FALSE(platform.isHttpsEnabled());

  platform.begin("Device3", true);
  TEST_ASSERT_EQUAL_STRING("Device3", platform.getDeviceName().c_str());
  TEST_ASSERT_TRUE(platform.isHttpsEnabled());

  // Test URL generation branches more thoroughly
  TEST_ASSERT_EQUAL_STRING("https://mock-device.local",
                           platform.getBaseUrl().c_str());

  platform.begin("Device4", false);
  TEST_ASSERT_EQUAL_STRING("http://mock-device.local",
                           platform.getBaseUrl().c_str());

  // Test registerWebRoute warning branches very specifically
  bool warning1Triggered = false;
  bool warning2Triggered = false;
  bool warning3Triggered = false;
  bool warning4Triggered = false;

  String warning1Msg, warning2Msg, warning3Msg, warning4Msg;

  platform.onWarn([&](const String &msg) {
    if (!warning1Triggered) {
      warning1Triggered = true;
      warning1Msg = msg;
    } else if (!warning2Triggered) {
      warning2Triggered = true;
      warning2Msg = msg;
    } else if (!warning3Triggered) {
      warning3Triggered = true;
      warning3Msg = msg;
    } else if (!warning4Triggered) {
      warning4Triggered = true;
      warning4Msg = msg;
    }
  });

  // Test the exact warning conditions
  // Condition 1: path.startsWith("/api/") - first part of OR
  platform.registerWebRoute(
      "/api/test1", [](WebRequest &req, WebResponse &res) {}, {AuthType::NONE},
      WebModule::WM_GET);
  TEST_ASSERT_TRUE(warning1Triggered);
  TEST_ASSERT_TRUE(warning1Msg.indexOf("/api/test1") >= 0);
  TEST_ASSERT_TRUE(warning1Msg.indexOf("registerWebRoute()") >= 0);

  // Condition 2: path.startsWith("api/") - second part of OR
  platform.registerWebRoute(
      "api/test2", [](WebRequest &req, WebResponse &res) {}, {AuthType::TOKEN},
      WebModule::WM_POST);
  TEST_ASSERT_TRUE(warning2Triggered);
  TEST_ASSERT_TRUE(warning2Msg.indexOf("api/test2") >= 0);

  // Test non-warning cases
  platform.registerWebRoute(
      "/normal", [](WebRequest &req, WebResponse &res) {}, {AuthType::NONE},
      WebModule::WM_GET);
  platform.registerWebRoute(
      "normal2", [](WebRequest &req, WebResponse &res) {}, {AuthType::SESSION},
      WebModule::WM_PUT);

  // These should not trigger additional warnings
  TEST_ASSERT_FALSE(warning3Triggered);
  TEST_ASSERT_FALSE(warning4Triggered);

  // Test edge cases for the warning conditions
  platform.registerWebRoute(
      "/api_but_not_slash", [](WebRequest &req, WebResponse &res) {},
      {AuthType::LOCAL_ONLY}, WebModule::WM_DELETE);
  // This should NOT trigger a warning because it's "/api_" not "/api/"

  platform.registerWebRoute(
      "api_but_not_slash2", [](WebRequest &req, WebResponse &res) {},
      {AuthType::NONE}, WebModule::WM_GET);
  // This should NOT trigger a warning because it's "api_" not "api/"

  // Test route counting with the registered routes
  TEST_ASSERT_EQUAL(
      6, platform.getRouteCount()); // 2 warning routes + 4 normal routes

  // Test disableRoute boundary condition more precisely
  int initialCount = platform.getRouteCount();

  // Disable when count > 0
  platform.disableRoute("/some/path", WebModule::WM_GET);
  TEST_ASSERT_EQUAL(initialCount - 1, platform.getRouteCount());

  // Continue disabling until we reach 0
  while (platform.getRouteCount() > 0) {
    platform.disableRoute("/any/path", WebModule::WM_GET);
  }

  TEST_ASSERT_EQUAL(0, platform.getRouteCount());

  // Now disable when count == 0 (should remain 0)
  platform.disableRoute("/cannot/disable", WebModule::WM_GET);
  TEST_ASSERT_EQUAL(0, platform.getRouteCount());
}

// Additional tests for specific ternary operator edge cases
void test_ternary_operator_branches() {
  MockWebRequest req;
  MockWebResponse res;

  // Test lines 114-116: getParam ternary branches
  // We need to test both sides of: mockParams.count(stdName) ?
  // String(mockParams.at(stdName).c_str()) : String("");

  // Setup some test data
  req.setParam("exists", "found");
  req.setParam("", "empty_key_value");          // Edge case: empty key
  req.setParam("special@key", "special@value"); // Special characters

  // Test first branch (count > 0)
  TEST_ASSERT_EQUAL_STRING("found", req.getParam("exists").c_str());
  TEST_ASSERT_EQUAL_STRING("empty_key_value", req.getParam("").c_str());
  TEST_ASSERT_EQUAL_STRING("special@value",
                           req.getParam("special@key").c_str());

  // Test second branch (count == 0)
  TEST_ASSERT_EQUAL_STRING("", req.getParam("not_exists").c_str());
  TEST_ASSERT_EQUAL_STRING("", req.getParam("also_missing").c_str());
  TEST_ASSERT_EQUAL_STRING("", req.getParam("never_set").c_str());

  // Test lines 128-130: getHeader ternary branches
  req.setMockHeader("X-Custom", "custom_header_value");
  req.setMockHeader("", "header_with_empty_name");
  req.setMockHeader("X-Empty", ""); // Empty value

  // Test first branch (header exists)
  TEST_ASSERT_EQUAL_STRING("custom_header_value",
                           req.getHeader("X-Custom").c_str());
  TEST_ASSERT_EQUAL_STRING("header_with_empty_name", req.getHeader("").c_str());
  TEST_ASSERT_EQUAL_STRING(
      "", req.getHeader("X-Empty").c_str()); // Empty but exists

  // Test second branch (header doesn't exist)
  TEST_ASSERT_EQUAL_STRING("", req.getHeader("Missing-Header").c_str());
  TEST_ASSERT_EQUAL_STRING("", req.getHeader("X-Not-Set").c_str());

  // Test lines 136-138: getJsonParam ternary branches
  req.setJsonParam("json_exists", "{\"test\": true}");
  req.setJsonParam("", "{\"empty_key\": true}"); // Empty key edge case
  req.setJsonParam("empty_json", "");            // Empty JSON

  // Test first branch (JSON param exists)
  TEST_ASSERT_EQUAL_STRING("{\"test\": true}",
                           req.getJsonParam("json_exists").c_str());
  TEST_ASSERT_EQUAL_STRING("{\"empty_key\": true}",
                           req.getJsonParam("").c_str());
  TEST_ASSERT_EQUAL_STRING("", req.getJsonParam("empty_json").c_str());

  // Test second branch (JSON param doesn't exist)
  TEST_ASSERT_EQUAL_STRING("", req.getJsonParam("missing_json").c_str());
  TEST_ASSERT_EQUAL_STRING("", req.getJsonParam("never_set_json").c_str());

  // Test lines 149: getAllParams ternary operations in loop
  // This tests the String conversion ternaries inside the loop
  auto allParams = req.getAllParams();

  // Verify all set parameters are returned with proper string conversion
  TEST_ASSERT_TRUE(allParams.find("exists") != allParams.end());
  TEST_ASSERT_EQUAL_STRING("found", allParams["exists"].c_str());
  TEST_ASSERT_TRUE(allParams.find("") != allParams.end());
  TEST_ASSERT_EQUAL_STRING("empty_key_value", allParams[""].c_str());
  TEST_ASSERT_TRUE(allParams.find("special@key") != allParams.end());
  TEST_ASSERT_EQUAL_STRING("special@value", allParams["special@key"].c_str());

  // Test lines 159: setMatchedRoute ternary
  // routePattern ? String(routePattern) : ""

  // Test first branch (routePattern != nullptr)
  req.setMatchedRoute("/users/{id}");
  // We can't directly test the internal state, but we can test that the setter
  // works

  // Test second branch (routePattern == nullptr)
  req.setMatchedRoute(nullptr);
  // This should handle null pointers gracefully

  // Test lines 208-210: MockWebResponse getHeader ternary
  res.setHeader("Response-Header", "response_value");
  res.setHeader("", "empty_name_response");
  res.setHeader("Empty-Value-Header", "");

  // Test first branch (header exists)
  TEST_ASSERT_EQUAL_STRING("response_value",
                           res.getHeader("Response-Header").c_str());
  TEST_ASSERT_EQUAL_STRING("empty_name_response", res.getHeader("").c_str());
  TEST_ASSERT_EQUAL_STRING("", res.getHeader("Empty-Value-Header").c_str());

  // Test second branch (header doesn't exist)
  TEST_ASSERT_EQUAL_STRING("",
                           res.getHeader("Missing-Response-Header").c_str());
  TEST_ASSERT_EQUAL_STRING("", res.getHeader("X-Not-In-Response").c_str());
}

// Test string conversion edge cases that might affect coverage
void test_string_conversion_edge_cases() {
  MockWebRequest req;

  // Test various string edge cases that might trigger different code paths

  // Unicode strings
  req.setParam("unicode", "测试");
  req.setParam("emoji", "🚀");
  req.setParam("mixed", "Test测试🚀123");

  TEST_ASSERT_EQUAL_STRING("测试", req.getParam("unicode").c_str());
  TEST_ASSERT_EQUAL_STRING("🚀", req.getParam("emoji").c_str());
  TEST_ASSERT_EQUAL_STRING("Test测试🚀123", req.getParam("mixed").c_str());

  // Very long strings that might trigger different allocation paths
  String longString = "";
  for (int i = 0; i < 1000; i++) {
    longString += "a";
  }

  req.setParam("long", longString);
  TEST_ASSERT_EQUAL_STRING(longString.c_str(), req.getParam("long").c_str());

  // Strings with null characters (edge case)
  req.setParam("with_null", "before\0after");
  // Note: This will likely be truncated at null, but tests the conversion

  // JSON edge cases
  req.setJsonParam("malformed", "{not json");
  req.setJsonParam("nested", "{\"a\":{\"b\":{\"c\":\"deep\"}}}");
  req.setJsonParam("array", "[1,2,3,{\"nested\":true}]");

  TEST_ASSERT_EQUAL_STRING("{not json", req.getJsonParam("malformed").c_str());
  TEST_ASSERT_EQUAL_STRING("{\"a\":{\"b\":{\"c\":\"deep\"}}}",
                           req.getJsonParam("nested").c_str());
  TEST_ASSERT_EQUAL_STRING("[1,2,3,{\"nested\":true}]",
                           req.getJsonParam("array").c_str());

  // Header edge cases
  req.setMockHeader("Long-Header-Name-That-Might-Cause-Issues",
                    "long_value_that_also_might_cause_allocation_issues");
  TEST_ASSERT_EQUAL_STRING(
      "long_value_that_also_might_cause_allocation_issues",
      req.getHeader("Long-Header-Name-That-Might-Cause-Issues").c_str());

  // Response string conversion edge cases
  MockWebResponse res;

  // Very long content
  String longContent = "<!DOCTYPE html><html><body>";
  for (int i = 0; i < 100; i++) {
    longContent +=
        "<p>This is paragraph " + String(i) + " with some content.</p>";
  }
  longContent += "</body></html>";

  res.setContent(longContent, "text/html");
  TEST_ASSERT_EQUAL_STRING(longContent.c_str(), res.getContent().c_str());
  TEST_ASSERT_EQUAL(longContent.length(), res.getContentLength());

  // Unicode in headers and content
  res.setHeader("X-Unicode", "测试值");
  res.setContent("测试内容🚀", "text/plain; charset=utf-8");

  TEST_ASSERT_EQUAL_STRING("测试值", res.getHeader("X-Unicode").c_str());
  TEST_ASSERT_EQUAL_STRING("测试内容🚀", res.getContent().c_str());
  TEST_ASSERT_EQUAL_STRING("text/plain; charset=utf-8",
                           res.getMimeType().c_str());
}

// These functions should be included in the main test runner
// They will be added to register_mock_web_platform_tests() in the main file