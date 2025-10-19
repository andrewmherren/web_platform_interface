#include "test_web_request.h"
#include <ArduinoFake.h>
#include <interface/web_request.h>
#include <unity.h>

using namespace fakeit;

void test_web_request_constants() {
  // Test that COMMON_HTTP_HEADERS is defined and has reasonable content
  TEST_ASSERT_NOT_NULL(COMMON_HTTP_HEADERS);
  TEST_ASSERT_TRUE(COMMON_HTTP_HEADERS_COUNT > 0);

  // Test that we can access the first header (should be non-null)
  if (COMMON_HTTP_HEADERS_COUNT > 0) {
    TEST_ASSERT_NOT_NULL(COMMON_HTTP_HEADERS[0]);
    // Verify it's a real string (has length > 0)
    TEST_ASSERT_TRUE(strlen(COMMON_HTTP_HEADERS[0]) > 0);
  }

  // Test bounds - accessing exactly at count should be safe if properly
  // null-terminated array But we won't access beyond COMMON_HTTP_HEADERS_COUNT
  // to avoid potential issues
}

void test_web_request_method_enum_coverage() {
  // Test WebModule::Method enum coverage in context of WebRequest
  // This ensures all enum values are handled properly

  // Create a simple validation that our enum values are distinct
  TEST_ASSERT_TRUE(WebModule::WM_GET != WebModule::WM_POST);
  TEST_ASSERT_TRUE(WebModule::WM_PUT != WebModule::WM_PATCH);
  TEST_ASSERT_TRUE(WebModule::WM_DELETE != WebModule::WM_GET);

  // Test that enum values are in expected ranges
  TEST_ASSERT_TRUE(WebModule::WM_GET >= 0);
  TEST_ASSERT_TRUE(WebModule::WM_POST >= 0);
  TEST_ASSERT_TRUE(WebModule::WM_PUT >= 0);
  TEST_ASSERT_TRUE(WebModule::WM_DELETE >= 0);
  TEST_ASSERT_TRUE(WebModule::WM_PATCH >= 0);
}

void test_web_request_auth_context_integration() {
  // Test AuthContext integration with WebRequest concepts
  AuthContext ctx;

  // Test default state
  TEST_ASSERT_FALSE(ctx.isAuthenticated);
  TEST_ASSERT_EQUAL_STRING("", ctx.username.c_str());
  TEST_ASSERT_EQUAL_STRING("", ctx.sessionId.c_str());

  // Test modification
  ctx.isAuthenticated = true;
  ctx.username = "testuser";
  ctx.sessionId = "session123";
  ctx.authenticatedVia = AuthType::SESSION;

  TEST_ASSERT_TRUE(ctx.isAuthenticated);
  TEST_ASSERT_EQUAL_STRING("testuser", ctx.username.c_str());
  TEST_ASSERT_EQUAL_STRING("session123", ctx.sessionId.c_str());
  TEST_ASSERT_TRUE(ctx.authenticatedVia == AuthType::SESSION);
}

// Registration function to run all web request tests
void register_web_request_tests() {
  RUN_TEST(test_web_request_constants);
  RUN_TEST(test_web_request_method_enum_coverage);
  RUN_TEST(test_web_request_auth_context_integration);
}