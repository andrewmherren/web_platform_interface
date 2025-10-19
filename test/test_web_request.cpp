#include "test_web_request.h"
#include <interface/auth_types.h>
#include <interface/string_compat.h>
#include <interface/web_module_types.h>
#include <interface/web_request.h>
#include <unity.h>


void test_web_request_constants() {
  // Test that the COMMON_HTTP_HEADERS array is properly defined
  TEST_ASSERT_NOT_NULL(COMMON_HTTP_HEADERS);
  TEST_ASSERT_GREATER_THAN(0, COMMON_HTTP_HEADERS_COUNT);

  // Test some expected headers are present
  bool found_host = false;
  bool found_user_agent = false;
  bool found_content_type = false;

  for (size_t i = 0; i < COMMON_HTTP_HEADERS_COUNT; i++) {
    TEST_ASSERT_NOT_NULL(COMMON_HTTP_HEADERS[i]);
    String header = String(COMMON_HTTP_HEADERS[i]);

    if (header.equals("Host"))
      found_host = true;
    if (header.equals("User-Agent"))
      found_user_agent = true;
    if (header.equals("Content-Type"))
      found_content_type = true;
  }

  TEST_ASSERT_TRUE(found_host);
  TEST_ASSERT_TRUE(found_user_agent);
  TEST_ASSERT_TRUE(found_content_type);
}

void test_auth_context_construction() {
  // Test AuthContext can be constructed and has expected defaults
  AuthContext context;

  // Test basic properties exist (these are part of the interface)
  TEST_ASSERT_FALSE(context.isAuthenticated);
  TEST_ASSERT_TRUE(StringUtils::isStringEmpty(context.username));
  TEST_ASSERT_TRUE(StringUtils::isStringEmpty(context.token));
  TEST_ASSERT_TRUE(StringUtils::isStringEmpty(context.sessionId));
  TEST_ASSERT_EQUAL(AuthType::NONE, context.authenticatedVia);
  TEST_ASSERT_EQUAL(0, context.authenticatedAt);
}

void test_auth_context_assignment() {
  // Test AuthContext can be assigned values
  AuthContext context;
  context.isAuthenticated = true;
  context.username = "testuser";
  context.token = "testtoken";
  context.sessionId = "testsession";
  context.authenticatedVia = AuthType::SESSION;
  context.authenticatedAt = 12345;

  TEST_ASSERT_TRUE(context.isAuthenticated);
  TEST_ASSERT_EQUAL_STRING("testuser", context.username.c_str());
  TEST_ASSERT_EQUAL_STRING("testtoken", context.token.c_str());
  TEST_ASSERT_EQUAL_STRING("testsession", context.sessionId.c_str());
  TEST_ASSERT_EQUAL(AuthType::SESSION, context.authenticatedVia);
  TEST_ASSERT_EQUAL(12345, context.authenticatedAt);
}

void test_auth_context_helper_methods() {
  // Test helper methods in AuthContext
  AuthContext context;

  // Test hasValidSession - should be false initially
  TEST_ASSERT_FALSE(context.hasValidSession());

  // Set up valid session
  context.isAuthenticated = true;
  context.authenticatedVia = AuthType::SESSION;
  context.sessionId = "valid-session-id";
  TEST_ASSERT_TRUE(context.hasValidSession());

  // Test hasValidToken - should be false initially
  context.clear(); // Reset
  TEST_ASSERT_FALSE(context.hasValidToken());

  // Set up valid token
  context.isAuthenticated = true;
  context.authenticatedVia = AuthType::TOKEN;
  context.token = "valid-token";
  TEST_ASSERT_TRUE(context.hasValidToken());

  // Test clear method
  context.clear();
  TEST_ASSERT_FALSE(context.isAuthenticated);
  TEST_ASSERT_EQUAL(AuthType::NONE, context.authenticatedVia);
  TEST_ASSERT_TRUE(StringUtils::isStringEmpty(context.sessionId));
  TEST_ASSERT_TRUE(StringUtils::isStringEmpty(context.token));
  TEST_ASSERT_TRUE(StringUtils::isStringEmpty(context.username));
  TEST_ASSERT_EQUAL(0, context.authenticatedAt);
}

// Registration function to run all web request tests
void register_web_request_tests() {
  RUN_TEST(test_web_request_constants);
  RUN_TEST(test_auth_context_construction);
  RUN_TEST(test_auth_context_assignment);
  RUN_TEST(test_auth_context_helper_methods);
}
