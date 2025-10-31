#include "../../include/testing/mocks.h"
#include <ArduinoFake.h>
#include <unity.h>

// Include mock infrastructure tests
#include <testing/mock_web_platform.h>

// Test module base path functionality in MockWebRequest instead
void test_mock_platform_module_base_path() {
  MockWebRequest mockReq;

  // Default base path should be empty
  TEST_ASSERT_EQUAL_STRING("", mockReq.getModuleBasePath().c_str());

  // Test setting and getting the module base path
  mockReq.setModuleBasePath("/test/module");
  TEST_ASSERT_EQUAL_STRING("/test/module", mockReq.getModuleBasePath().c_str());
}

// Test MockWebRequest basic functionality
void test_mock_web_request_basic_operations() {
  MockWebRequest mockReq;

  // Test default path should be "/"
  TEST_ASSERT_EQUAL_STRING("/", mockReq.getPath().c_str());

  // Test changing path
  mockReq.setPath("/test/path");
  TEST_ASSERT_EQUAL_STRING("/test/path", mockReq.getPath().c_str());

  // Test parameter handling
  mockReq.setParam("key1", "value1");
  mockReq.setParam("key2", "value2");

  TEST_ASSERT_EQUAL_STRING("value1", mockReq.getParam("key1").c_str());
  TEST_ASSERT_EQUAL_STRING("value2", mockReq.getParam("key2").c_str());
  TEST_ASSERT_EQUAL_STRING("", mockReq.getParam("nonexistent").c_str());

  // Test getAllParams method
  std::map<String, String> allParams = mockReq.getAllParams();
  TEST_ASSERT_EQUAL(2, allParams.size());
  TEST_ASSERT_EQUAL_STRING("value1", allParams["key1"].c_str());
  TEST_ASSERT_EQUAL_STRING("value2", allParams["key2"].c_str());

  // Test body handling
  mockReq.setBody("test body content");
  TEST_ASSERT_EQUAL_STRING("test body content", mockReq.getBody().c_str());

  // Test method handling (default should be GET)
  TEST_ASSERT_EQUAL(WebModule::WM_GET, mockReq.getMethod());

  // Test changing method
  mockReq.setMethod(WebModule::WM_POST);
  TEST_ASSERT_EQUAL(WebModule::WM_POST, mockReq.getMethod());
}

void test_mock_web_request_headers_and_auth() {
  MockWebRequest mockReq;

  // Test header handling if implemented
  if (std::string(typeid(mockReq).name()).find("MockWebRequest") !=
      std::string::npos) {
    mockReq.setMockHeader("Content-Type", "application/json");
    mockReq.setMockHeader("Authorization", "Bearer token123");

    TEST_ASSERT_EQUAL_STRING("application/json",
                             mockReq.getHeader("Content-Type").c_str());
    TEST_ASSERT_EQUAL_STRING("Bearer token123",
                             mockReq.getHeader("Authorization").c_str());
    TEST_ASSERT_EQUAL_STRING("", mockReq.getHeader("NonExistent").c_str());
  }

  // Test authentication context
  const AuthContext &authCtx = mockReq.getAuthContext();
  TEST_ASSERT_FALSE(authCtx.isAuthenticated); // Default should be false

  // Test setting authentication
  mockReq.setAuthContext(true, "testuser");
  const AuthContext &newAuthCtx = mockReq.getAuthContext();
  TEST_ASSERT_TRUE(newAuthCtx.isAuthenticated);
  TEST_ASSERT_EQUAL_STRING("testuser", newAuthCtx.username.c_str());
  TEST_ASSERT_TRUE(newAuthCtx.authenticatedVia == AuthType::SESSION);
  TEST_ASSERT_EQUAL_STRING("test_session", newAuthCtx.sessionId.c_str());

  // Test client IP if implemented
  if (std::string(typeid(mockReq).name()).find("MockWebRequest") !=
      std::string::npos) {
    TEST_ASSERT_EQUAL_STRING("127.0.0.1", mockReq.getClientIp().c_str());
  }
}

// Test MockWebResponse basic functionality
void test_mock_web_response_basic_operations() {
  MockWebResponse mockRes;

  // Test content setting
  mockRes.setContent("Hello World", "text/plain");
  TEST_ASSERT_EQUAL_STRING("Hello World", mockRes.getContent().c_str());
  TEST_ASSERT_EQUAL_STRING("text/plain", mockRes.getContentType().c_str());
  TEST_ASSERT_EQUAL(11, mockRes.getContentLength()); // "Hello World" = 11 chars

  // Test PROGMEM content if implemented
  const char *testProgmem = "PROGMEM test content";
  mockRes.setProgmemContent(testProgmem, "application/javascript");
  TEST_ASSERT_EQUAL_STRING("PROGMEM test content",
                           mockRes.getContent().c_str());
  TEST_ASSERT_EQUAL_STRING("application/javascript",
                           mockRes.getContentType().c_str());

  // Test status codes if implemented
  if (std::string(typeid(mockRes).name()).find("MockWebResponse") !=
      std::string::npos) {
    // Default status should be 200 OK
    TEST_ASSERT_EQUAL(200, mockRes.getStatusCode());

    // Test changing status
    mockRes.setStatus(404);
    TEST_ASSERT_EQUAL(404, mockRes.getStatusCode());
  }
}

void test_mock_web_response_headers_and_redirect() {
  MockWebResponse mockRes;

  // Test header handling
  mockRes.setHeader("Cache-Control", "no-cache");
  mockRes.setHeader("X-Custom-Header", "custom-value");

  // Verify headers if getHeader is implemented
  if (std::string(typeid(mockRes).name()).find("MockWebResponse") !=
      std::string::npos) {
    TEST_ASSERT_EQUAL_STRING("no-cache",
                             mockRes.getHeader("Cache-Control").c_str());
    TEST_ASSERT_EQUAL_STRING("custom-value",
                             mockRes.getHeader("X-Custom-Header").c_str());
    TEST_ASSERT_EQUAL_STRING("", mockRes.getHeader("NonExistent").c_str());
  }

  // Test redirect if implemented
  if (std::string(typeid(mockRes).name()).find("MockWebResponse") !=
      std::string::npos) {
    // Test with explicit status code
    mockRes.redirect("https://example.com", 301);
    TEST_ASSERT_EQUAL(301, mockRes.getStatusCode());
    TEST_ASSERT_EQUAL_STRING("https://example.com",
                             mockRes.getHeader("Location").c_str());

    // Test default redirect status (302)
    mockRes.redirect("/new-path");
    TEST_ASSERT_EQUAL(302, mockRes.getStatusCode());
    TEST_ASSERT_EQUAL_STRING("/new-path",
                             mockRes.getHeader("Location").c_str());
  }
}

// Registration function to run all mock tests
void register_mock_tests() {
  RUN_TEST(test_mock_platform_module_base_path);
  RUN_TEST(test_mock_web_request_basic_operations);
  RUN_TEST(test_mock_web_request_headers_and_auth);
  RUN_TEST(test_mock_web_response_basic_operations);
  RUN_TEST(test_mock_web_response_headers_and_redirect);
}
