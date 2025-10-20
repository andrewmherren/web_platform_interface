#include "test_web_request.h"
#include <testing/mock_web_platform.h>
#include <unity.h>

void test_web_request_path_access() {
  MockWebRequest request("/test/path");
  TEST_ASSERT_EQUAL_STRING("/test/path", request.getPath().c_str());

  // Test changing path
  request.setPath("/new/path");
  TEST_ASSERT_EQUAL_STRING("/new/path", request.getPath().c_str());
}

void test_web_request_query_params() {
  MockWebRequest request;

  // Test setting and getting parameters
  request.setParam("param1", "value1");
  request.setParam("param2", "value2");
  request.setParam("empty", "");

  TEST_ASSERT_EQUAL_STRING("value1", request.getParam("param1").c_str());
  TEST_ASSERT_EQUAL_STRING("value2", request.getParam("param2").c_str());
  TEST_ASSERT_EQUAL_STRING("", request.getParam("empty").c_str());
  TEST_ASSERT_EQUAL_STRING("", request.getParam("nonexistent").c_str());

  // Test getAllParams
  std::map<String, String> allParams = request.getAllParams();
  TEST_ASSERT_EQUAL(3, allParams.size());
}

void test_web_request_headers() {
  MockWebRequest request;

  // Test setting and getting headers
  request.setMockHeader("Host", "localhost");
  request.setMockHeader("Content-Type", "application/json");
  request.setMockHeader("Authorization", "Bearer token123");
  request.setMockHeader("User-Agent", "Test-Client/1.0");

  TEST_ASSERT_EQUAL_STRING("localhost", request.getHeader("Host").c_str());
  TEST_ASSERT_EQUAL_STRING("application/json",
                           request.getHeader("Content-Type").c_str());
  TEST_ASSERT_EQUAL_STRING("Bearer token123",
                           request.getHeader("Authorization").c_str());
  TEST_ASSERT_EQUAL_STRING("Test-Client/1.0",
                           request.getHeader("User-Agent").c_str());
  TEST_ASSERT_EQUAL_STRING("", request.getHeader("NonExistent").c_str());
}

void test_web_request_auth_context() {
  MockWebRequest request;

  // Test default authentication state
  const AuthContext &defaultContext = request.getAuthContext();
  TEST_ASSERT_FALSE(defaultContext.isAuthenticated);

  // Test setting authentication via convenience method
  request.setAuthContext(true, "test_user");
  const AuthContext &authContext = request.getAuthContext();
  TEST_ASSERT_TRUE(authContext.isAuthenticated);
  TEST_ASSERT_EQUAL_STRING("test_user", authContext.username.c_str());
  TEST_ASSERT_EQUAL(AuthType::SESSION, authContext.authenticatedVia);
  TEST_ASSERT_EQUAL_STRING("test_session", authContext.sessionId.c_str());

  // Test setting authentication via full context
  AuthContext customContext;
  customContext.isAuthenticated = true;
  customContext.authenticatedVia = AuthType::TOKEN;
  customContext.username = "api_user";
  customContext.token = "abc123";
  customContext.authenticatedAt = 12345;

  request.setAuthContext(customContext);
  const AuthContext &resultContext = request.getAuthContext();
  TEST_ASSERT_TRUE(resultContext.isAuthenticated);
  TEST_ASSERT_EQUAL(AuthType::TOKEN, resultContext.authenticatedVia);
  TEST_ASSERT_EQUAL_STRING("api_user", resultContext.username.c_str());
  TEST_ASSERT_EQUAL_STRING("abc123", resultContext.token.c_str());
  TEST_ASSERT_EQUAL(12345UL, resultContext.authenticatedAt);
}

void test_web_request_route_matching() {
  MockWebRequest request;

  // Set up route parameters for testing
  request.setParam("id", "123");
  request.setParam("name", "test_item");
  request.setParam("category", "widgets");

  // Test route parameter extraction (MockWebRequest uses param lookup)
  TEST_ASSERT_EQUAL_STRING("123", request.getRouteParameter("id").c_str());
  TEST_ASSERT_EQUAL_STRING("test_item",
                           request.getRouteParameter("name").c_str());
  TEST_ASSERT_EQUAL_STRING("widgets",
                           request.getRouteParameter("category").c_str());
  TEST_ASSERT_EQUAL_STRING("",
                           request.getRouteParameter("nonexistent").c_str());

  // Test setting matched route pattern
  request.setMatchedRoute("/items/{id}/details/{name}");
  // The pattern is stored but actual extraction still uses params in mock
  TEST_ASSERT_EQUAL_STRING("123", request.getRouteParameter("id").c_str());
}

void test_web_request_module_base_path() {
  MockWebRequest request;

  // Test default module base path
  TEST_ASSERT_EQUAL_STRING("", request.getModuleBasePath().c_str());

  // Test setting module base path
  request.setModuleBasePath("/test");
  TEST_ASSERT_EQUAL_STRING("/test", request.getModuleBasePath().c_str());

  // Test changing module base path
  request.setModuleBasePath("/api/v1");
  TEST_ASSERT_EQUAL_STRING("/api/v1", request.getModuleBasePath().c_str());

  // Test empty module base path
  request.setModuleBasePath("");
  TEST_ASSERT_EQUAL_STRING("", request.getModuleBasePath().c_str());
}

void test_web_request_method_access() {
  MockWebRequest request;

  // Test default method
  TEST_ASSERT_EQUAL(WebModule::WM_GET, request.getMethod());

  // Test setting different methods
  request.setMethod(WebModule::WM_POST);
  TEST_ASSERT_EQUAL(WebModule::WM_POST, request.getMethod());

  request.setMethod(WebModule::WM_PUT);
  TEST_ASSERT_EQUAL(WebModule::WM_PUT, request.getMethod());

  request.setMethod(WebModule::WM_DELETE);
  TEST_ASSERT_EQUAL(WebModule::WM_DELETE, request.getMethod());
}

void test_web_request_body_access() {
  MockWebRequest request;

  // Test default empty body
  TEST_ASSERT_EQUAL_STRING("", request.getBody().c_str());

  // Test setting body content
  request.setBody("{\"key\": \"value\"}");
  TEST_ASSERT_EQUAL_STRING("{\"key\": \"value\"}", request.getBody().c_str());

  // Test setting form data body
  request.setBody("name=test&value=123");
  TEST_ASSERT_EQUAL_STRING("name=test&value=123", request.getBody().c_str());

  // Test clearing body
  request.setBody("");
  TEST_ASSERT_EQUAL_STRING("", request.getBody().c_str());
}

void test_web_request_json_params() {
  MockWebRequest request;

  // Test setting and getting JSON parameters
  request.setJsonParam("name", "John Doe");
  request.setJsonParam("age", "30");
  request.setJsonParam("active", "true");

  TEST_ASSERT_EQUAL_STRING("John Doe", request.getJsonParam("name").c_str());
  TEST_ASSERT_EQUAL_STRING("30", request.getJsonParam("age").c_str());
  TEST_ASSERT_EQUAL_STRING("true", request.getJsonParam("active").c_str());
  TEST_ASSERT_EQUAL_STRING("", request.getJsonParam("nonexistent").c_str());

  // Test empty JSON param
  request.setJsonParam("empty", "");
  TEST_ASSERT_EQUAL_STRING("", request.getJsonParam("empty").c_str());
}

void test_web_request_client_ip() {
  MockWebRequest request;

  // Test default client IP (from mock)
  TEST_ASSERT_EQUAL_STRING("127.0.0.1", request.getClientIp().c_str());

  // Test setting custom client IP
  request.setClientIp("192.168.1.100");
  TEST_ASSERT_EQUAL_STRING("192.168.1.100", request.getClientIp().c_str());

  // Test setting another IP
  request.setClientIp("10.0.0.1");
  TEST_ASSERT_EQUAL_STRING("10.0.0.1", request.getClientIp().c_str());
}

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

void register_web_request_tests() {
  RUN_TEST(test_web_request_path_access);
  RUN_TEST(test_web_request_method_access);
  RUN_TEST(test_web_request_body_access);
  RUN_TEST(test_web_request_query_params);
  RUN_TEST(test_web_request_headers);
  RUN_TEST(test_web_request_json_params);
  RUN_TEST(test_web_request_auth_context);
  RUN_TEST(test_web_request_route_matching);
  RUN_TEST(test_web_request_module_base_path);
  RUN_TEST(test_web_request_client_ip);
  RUN_TEST(test_web_request_constants);
  RUN_TEST(test_auth_context_construction);
  RUN_TEST(test_auth_context_assignment);
  RUN_TEST(test_auth_context_helper_methods);
}
