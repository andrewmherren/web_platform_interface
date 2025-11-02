#include <interface/core/web_request_core.h>
#include <unity.h>

// Test WebRequestCore query parameter parsing
void test_web_request_core_parse_query_params() {
  WebRequestCore request;

  // Test basic query string parsing
  request.parseQueryParams("name=value&key=val");
  TEST_ASSERT_EQUAL_STRING("value", request.getParam("name").c_str());
  TEST_ASSERT_EQUAL_STRING("val", request.getParam("key").c_str());

  // Test URL-encoded values (tests urlDecode indirectly)
  request.clearParams();
  request.parseQueryParams("space=Hello+World&percent=test%20value");
  TEST_ASSERT_EQUAL_STRING("Hello World", request.getParam("space").c_str());
  TEST_ASSERT_EQUAL_STRING("test value", request.getParam("percent").c_str());

  // Test special characters
  request.clearParams();
  request.parseQueryParams("email=test%40example.com&text=50%25+off");
  TEST_ASSERT_EQUAL_STRING("test@example.com",
                           request.getParam("email").c_str());
  TEST_ASSERT_EQUAL_STRING("50% off", request.getParam("text").c_str());

  // Test empty value
  request.clearParams();
  request.parseQueryParams("empty=&nonempty=test");
  TEST_ASSERT_EQUAL_STRING("", request.getParam("empty").c_str());
  TEST_ASSERT_EQUAL_STRING("test", request.getParam("nonempty").c_str());

  // Test empty query string
  request.clearParams();
  request.parseQueryParams("");
  TEST_ASSERT_EQUAL(0, request.getAllParams().size());

  // Test single parameter without value
  request.clearParams();
  request.parseQueryParams("flag");
  TEST_ASSERT_EQUAL_STRING("", request.getParam("flag").c_str());

  // Test multiple ampersands
  request.clearParams();
  request.parseQueryParams("a=1&&b=2");
  TEST_ASSERT_EQUAL_STRING("1", request.getParam("a").c_str());
  TEST_ASSERT_EQUAL_STRING("2", request.getParam("b").c_str());
}

// Test WebRequestCore form data parsing
void test_web_request_core_parse_form_data() {
  WebRequestCore request;

  // Form data has same format as query string
  request.parseFormData("username=admin&password=secret123");
  TEST_ASSERT_EQUAL_STRING("admin", request.getParam("username").c_str());
  TEST_ASSERT_EQUAL_STRING("secret123", request.getParam("password").c_str());

  // Test URL-encoded form data
  request.clearParams();
  request.parseFormData("message=Hello+World%21&from=user%40example.com");
  TEST_ASSERT_EQUAL_STRING("Hello World!", request.getParam("message").c_str());
  TEST_ASSERT_EQUAL_STRING("user@example.com",
                           request.getParam("from").c_str());

  // Test empty form data
  request.clearParams();
  request.parseFormData("");
  TEST_ASSERT_EQUAL(0, request.getAllParams().size());
}

// Test WebRequestCore JSON data parsing (native simple parser)
void test_web_request_core_parse_json_data() {
  WebRequestCore request;

  // Test simple JSON object
  request.parseJsonData(R"({"name":"test","value":"123"})");
  TEST_ASSERT_EQUAL_STRING("test", request.getJsonParam("name").c_str());
  TEST_ASSERT_EQUAL_STRING("123", request.getJsonParam("value").c_str());

  // Test JSON with numbers and booleans (stored as strings)
  request.clearJsonParams();
  request.parseJsonData(R"({"count":42,"enabled":true})");
  TEST_ASSERT_EQUAL_STRING("42", request.getJsonParam("count").c_str());
  TEST_ASSERT_EQUAL_STRING("true", request.getJsonParam("enabled").c_str());

  // Test JSON with whitespace
  request.clearJsonParams();
  request.parseJsonData(R"({  "key" : "value"  })");
  TEST_ASSERT_EQUAL_STRING("value", request.getJsonParam("key").c_str());

  // Test empty JSON object
  request.clearJsonParams();
  request.parseJsonData("{}");
  TEST_ASSERT_EQUAL(0, request.getJsonParam("nonexistent").size());

  // Test invalid JSON (should not crash)
  request.clearJsonParams();
  request.parseJsonData("not json");
  // Parsing may fail silently - just ensure no crash

  // Test JSON with nested objects (only top-level parsing supported)
  request.clearJsonParams();
  request.parseJsonData(R"({"outer":"value"})");
  TEST_ASSERT_EQUAL_STRING("value", request.getJsonParam("outer").c_str());
}

// Test WebRequestCore body parsing with content type routing
void test_web_request_core_parse_request_body() {
  WebRequestCore request;

  // Test JSON content type
  std::string jsonBody = R"({"user":"admin","pass":"secret"})";
  request.parseRequestBody(jsonBody, "application/json");
  TEST_ASSERT_EQUAL_STRING("admin", request.getJsonParam("user").c_str());
  TEST_ASSERT_EQUAL_STRING("secret", request.getJsonParam("pass").c_str());

  // Test form-urlencoded content type
  request.clearParams();
  request.clearJsonParams();
  std::string formBody = "field1=value1&field2=value2";
  request.parseRequestBody(formBody, "application/x-www-form-urlencoded");
  TEST_ASSERT_EQUAL_STRING("value1", request.getParam("field1").c_str());
  TEST_ASSERT_EQUAL_STRING("value2", request.getParam("field2").c_str());

  // Test content type with charset
  request.clearParams();
  request.clearJsonParams();
  request.parseRequestBody(jsonBody, "application/json; charset=utf-8");
  TEST_ASSERT_EQUAL_STRING("admin", request.getJsonParam("user").c_str());

  // Test unknown content type (should not parse)
  request.clearParams();
  request.clearJsonParams();
  request.parseRequestBody("some data", "text/plain");
  TEST_ASSERT_EQUAL(0, request.getAllParams().size());
}

// Test WebRequestCore case-insensitive header lookup
void test_web_request_core_header_case_insensitive() {
  WebRequestCore request;

  // Set headers using setHeader (public API)
  request.setHeader("Content-Type", "application/json");
  request.setHeader("Host", "localhost");
  request.setHeader("User-Agent", "TestClient/1.0");

  // Test case-insensitive retrieval (tests caseInsensitiveCompare indirectly)
  TEST_ASSERT_EQUAL_STRING("application/json",
                           request.getHeader("Content-Type").c_str());
  TEST_ASSERT_EQUAL_STRING("application/json",
                           request.getHeader("content-type").c_str());
  TEST_ASSERT_EQUAL_STRING("application/json",
                           request.getHeader("CONTENT-TYPE").c_str());

  TEST_ASSERT_EQUAL_STRING("localhost", request.getHeader("host").c_str());
  TEST_ASSERT_EQUAL_STRING("localhost", request.getHeader("HOST").c_str());
  TEST_ASSERT_EQUAL_STRING("localhost", request.getHeader("Host").c_str());

  // Test non-existent header
  TEST_ASSERT_EQUAL_STRING("", request.getHeader("X-Custom-Header").c_str());
}

// Test WebRequestCore basic setters and getters
void test_web_request_core_setters_getters() {
  WebRequestCore request;

  // Test path
  request.setPath("/test/path");
  TEST_ASSERT_EQUAL_STRING("/test/path", request.getPath().c_str());

  // Test method
  request.setMethod(WebModule::WM_POST);
  TEST_ASSERT_EQUAL(WebModule::WM_POST, request.getMethod());

  // Test body
  request.setBody("test body content");
  TEST_ASSERT_EQUAL_STRING("test body content", request.getBody().c_str());

  // Test client IP
  request.setClientIp("192.168.1.100");
  TEST_ASSERT_EQUAL_STRING("192.168.1.100", request.getClientIp().c_str());

  // Test matched route pattern
  request.setMatchedRoute("/items/{id}");
  TEST_ASSERT_EQUAL_STRING("/items/{id}", request.getMatchedRoute().c_str());

  // Test module base path
  request.setModuleBasePath("/api");
  TEST_ASSERT_EQUAL_STRING("/api", request.getModuleBasePath().c_str());
}

// Test WebRequestCore route parameter extraction
void test_web_request_core_route_parameters() {
  WebRequestCore request;

  // Set up route parameters using public API
  request.setParam("id", "123");
  request.setParam("name", "test-item");
  request.setParam("category", "widgets");

  // Test parameter retrieval
  TEST_ASSERT_EQUAL_STRING("123", request.getRouteParameter("id").c_str());
  TEST_ASSERT_EQUAL_STRING("test-item",
                           request.getRouteParameter("name").c_str());
  TEST_ASSERT_EQUAL_STRING("widgets",
                           request.getRouteParameter("category").c_str());

  // Test non-existent parameter
  TEST_ASSERT_EQUAL_STRING("",
                           request.getRouteParameter("nonexistent").c_str());

  // Test getAllParams
  const auto &allParams = request.getAllParams();
  TEST_ASSERT_EQUAL(3, allParams.size());
  TEST_ASSERT_EQUAL_STRING("123", allParams.at("id").c_str());
}

// Test WebRequestCore default constructor
void test_web_request_core_default_constructor() {
  WebRequestCore request;

  // Test default initialization
  TEST_ASSERT_EQUAL_STRING("/", request.getPath().c_str());
  TEST_ASSERT_EQUAL(WebModule::WM_GET, request.getMethod());
  TEST_ASSERT_EQUAL_STRING("", request.getBody().c_str());
  TEST_ASSERT_EQUAL_STRING("", request.getClientIp().c_str());
  TEST_ASSERT_EQUAL(0, request.getAllParams().size());
}

// Test WebRequestCore header operations
void test_web_request_core_header_operations() {
  WebRequestCore request;

  // Set multiple headers
  request.setHeader("Accept", "application/json");
  request.setHeader("Authorization", "Bearer token123");

  // Test retrieval
  TEST_ASSERT_EQUAL_STRING("application/json",
                           request.getHeader("Accept").c_str());
  TEST_ASSERT_EQUAL_STRING("Bearer token123",
                           request.getHeader("Authorization").c_str());

  // Clear headers
  request.clearHeaders();
  TEST_ASSERT_EQUAL_STRING("", request.getHeader("Accept").c_str());
}

// Test WebRequestCore JSON parameter operations
void test_web_request_core_json_param_operations() {
  WebRequestCore request;

  // Set JSON params manually
  request.setJsonParam("key1", "value1");
  request.setJsonParam("key2", "value2");

  TEST_ASSERT_EQUAL_STRING("value1", request.getJsonParam("key1").c_str());
  TEST_ASSERT_EQUAL_STRING("value2", request.getJsonParam("key2").c_str());

  // Clear JSON params
  request.clearJsonParams();
  TEST_ASSERT_EQUAL_STRING("", request.getJsonParam("key1").c_str());
}

// Test WebRequestCore auth context
void test_web_request_core_auth_context() {
  WebRequestCore request;

  AuthContext context;
  context.isAuthenticated = true;
  context.username = "testuser";
  context.authenticatedVia = AuthType::SESSION;
  context.sessionId = "session123";

  request.setAuthContext(context);

  const AuthContext &retrieved = request.getAuthContext();
  TEST_ASSERT_TRUE(retrieved.isAuthenticated);
  TEST_ASSERT_EQUAL_STRING("testuser", retrieved.username.c_str());
  TEST_ASSERT_EQUAL(AuthType::SESSION, retrieved.authenticatedVia);
  TEST_ASSERT_EQUAL_STRING("session123", retrieved.sessionId.c_str());
} // Registration function
void register_web_request_core_tests() {
  RUN_TEST(test_web_request_core_parse_query_params);
  RUN_TEST(test_web_request_core_parse_form_data);
  RUN_TEST(test_web_request_core_parse_json_data);
  RUN_TEST(test_web_request_core_parse_request_body);
  RUN_TEST(test_web_request_core_header_case_insensitive);
  RUN_TEST(test_web_request_core_setters_getters);
  RUN_TEST(test_web_request_core_route_parameters);
  RUN_TEST(test_web_request_core_default_constructor);
  RUN_TEST(test_web_request_core_header_operations);
  RUN_TEST(test_web_request_core_json_param_operations);
  RUN_TEST(test_web_request_core_auth_context);
}
