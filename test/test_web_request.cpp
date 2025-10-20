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
}
