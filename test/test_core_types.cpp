#include <ArduinoFake.h>
#include <ArduinoJson.h>
#include <unity.h>

// Test the core types and utilities
#include <interface/auth_types.h>
#include <interface/openapi_types.h>
#include <interface/web_module_types.h>

// Test OpenAPIFactory if available
#ifdef OPENAPI_ENABLED
#include <interface/openapi_factory.h>
#endif

// Include mock infrastructure tests
#include <testing/mock_web_platform.h>
#include <testing/utils/string_compat.h>

// Forward declare mock infrastructure test functions
void test_mock_web_request_basic_operations();
void test_mock_web_request_headers_and_auth();
void test_mock_web_response_basic_operations();
void test_mock_web_response_headers_and_redirect();
void test_string_compatibility();
void test_json_serialization();
void setUpMockInfra();
void tearDownMockInfra();

void setUp() {
  ArduinoFakeReset();
  setUpMockInfra();
}

void tearDown() { tearDownMockInfra(); }

// Test basic Web Module types
void test_web_module_methods() {
  // Test WebModule::Method enum values
  TEST_ASSERT_EQUAL(0, static_cast<int>(WebModule::WM_GET));
  TEST_ASSERT_EQUAL(1, static_cast<int>(WebModule::WM_POST));
  TEST_ASSERT_EQUAL(2, static_cast<int>(WebModule::WM_PUT));
  TEST_ASSERT_EQUAL(3, static_cast<int>(WebModule::WM_DELETE));
  TEST_ASSERT_EQUAL(4, static_cast<int>(WebModule::WM_PATCH));

  // Verify methods are distinct
  TEST_ASSERT_TRUE(WebModule::WM_GET != WebModule::WM_POST);
  TEST_ASSERT_TRUE(WebModule::WM_PUT != WebModule::WM_DELETE);
  TEST_ASSERT_TRUE(WebModule::WM_PATCH != WebModule::WM_GET);
}

void test_auth_types() {
  // Test AuthType enum values
  TEST_ASSERT_TRUE(AuthType::NONE != AuthType::SESSION);
  TEST_ASSERT_TRUE(AuthType::TOKEN != AuthType::PAGE_TOKEN);
  TEST_ASSERT_TRUE(AuthType::LOCAL_ONLY != AuthType::NONE);

  // Test we can create a vector of auth types
  std::vector<AuthType> authTypes = {AuthType::NONE, AuthType::SESSION};
  TEST_ASSERT_EQUAL(2, authTypes.size());
  TEST_ASSERT_TRUE(authTypes[0] == AuthType::NONE);
  TEST_ASSERT_TRUE(authTypes[1] == AuthType::SESSION);
}

void test_openapi_documentation_basic_operations() {
  // Test default construction
  OpenAPIDocumentation doc;
  TEST_ASSERT_EQUAL_STRING("", doc.getSummary().c_str());
  TEST_ASSERT_EQUAL_STRING("", doc.getDescription().c_str());

  // Test parameterized construction
  OpenAPIDocumentation doc2("Summary", "Description");
  TEST_ASSERT_EQUAL_STRING("Summary", doc2.getSummary().c_str());
  TEST_ASSERT_EQUAL_STRING("Description", doc2.getDescription().c_str());

  // Test builder pattern methods
  doc2.withResponseExample("{\"test\": true}");
  doc2.withResponseSchema("{\"type\": \"object\"}");

  TEST_ASSERT_TRUE(doc2.getResponseExample().indexOf("test") >= 0);
  TEST_ASSERT_TRUE(doc2.getResponseSchema().indexOf("object") >= 0);
}

#ifdef OPENAPI_ENABLED
void test_openapi_factory_create_documentation() {
  std::vector<String> tags;
  tags.push_back("test");
  tags.push_back("api");

  OpenAPIDocumentation doc = OpenAPIFactory::create(
      "Test Summary", "Test Description", "testOperation", tags);

  TEST_ASSERT_EQUAL_STRING("Test Summary", doc.getSummary().c_str());
  TEST_ASSERT_EQUAL_STRING("Test Description", doc.getDescription().c_str());
  TEST_ASSERT_EQUAL_STRING("testOperation", doc.getOperationId().c_str());

  const std::vector<String> &docTags = doc.getTags();
  TEST_ASSERT_EQUAL(2, docTags.size());
  if (docTags.size() >= 2) {
    TEST_ASSERT_EQUAL_STRING("test", docTags[0].c_str());
    TEST_ASSERT_EQUAL_STRING("api", docTags[1].c_str());
  }
}

void test_openapi_factory_success_response() {
  String response =
      OpenAPIFactory::createSuccessResponse("Test response description");

  // Should be valid JSON-like structure
  TEST_ASSERT_TRUE(response.indexOf("type") >= 0);
  TEST_ASSERT_TRUE(response.indexOf("object") >= 0);
  TEST_ASSERT_TRUE(response.indexOf("Test response description") >= 0);
}
#endif

void test_auth_context_basic_operations() {
  // Test default construction
  AuthContext ctx;
  TEST_ASSERT_FALSE(ctx.isAuthenticated);
  TEST_ASSERT_EQUAL_STRING("", ctx.username.c_str());

  // Test context modification
  ctx.isAuthenticated = true;
  ctx.username = "newuser";
  ctx.authenticatedVia = AuthType::SESSION;
  ctx.sessionId = "session123";

  TEST_ASSERT_TRUE(ctx.isAuthenticated);
  TEST_ASSERT_EQUAL_STRING("newuser", ctx.username.c_str());
  TEST_ASSERT_TRUE(ctx.authenticatedVia == AuthType::SESSION);
  TEST_ASSERT_EQUAL_STRING("session123", ctx.sessionId.c_str());
}

void test_auth_requirements_collections() {
  // Test AuthRequirements as vector
  std::vector<AuthType> requirements;
  requirements.push_back(AuthType::SESSION);
  requirements.push_back(AuthType::TOKEN);

  TEST_ASSERT_EQUAL(2, requirements.size());
  TEST_ASSERT_TRUE(requirements[0] == AuthType::SESSION);
  TEST_ASSERT_TRUE(requirements[1] == AuthType::TOKEN);

  // Test finding auth types
  bool hasSession = false;
  bool hasToken = false;
  for (const auto &auth : requirements) {
    if (auth == AuthType::SESSION)
      hasSession = true;
    if (auth == AuthType::TOKEN)
      hasToken = true;
  }

  TEST_ASSERT_TRUE(hasSession);
  TEST_ASSERT_TRUE(hasToken);
}

#ifdef NATIVE_PLATFORM
int main(int argc, char **argv) {
  UNITY_BEGIN();

  RUN_TEST(test_web_module_methods);
  RUN_TEST(test_auth_types);
  RUN_TEST(test_openapi_documentation_basic_operations);

#ifdef OPENAPI_ENABLED
  RUN_TEST(test_openapi_factory_create_documentation);
  RUN_TEST(test_openapi_factory_success_response);
#endif

  RUN_TEST(test_auth_context_basic_operations);
  RUN_TEST(test_auth_requirements_collections);

  // Run mock infrastructure tests
  RUN_TEST(test_mock_web_request_basic_operations);
  RUN_TEST(test_mock_web_request_headers_and_auth);
  RUN_TEST(test_mock_web_response_basic_operations);
  RUN_TEST(test_mock_web_response_headers_and_redirect);
  RUN_TEST(test_string_compatibility);
  RUN_TEST(test_json_serialization);

  UNITY_END();
  return 0;
}
#else
void setup() {
  UNITY_BEGIN();

  RUN_TEST(test_web_module_methods);
  RUN_TEST(test_auth_types);
  RUN_TEST(test_openapi_documentation_basic_operations);

#ifdef OPENAPI_ENABLED
  RUN_TEST(test_openapi_factory_create_documentation);
  RUN_TEST(test_openapi_factory_success_response);
#endif

  RUN_TEST(test_auth_context_basic_operations);
  RUN_TEST(test_auth_requirements_collections);

  // Run mock infrastructure tests
  RUN_TEST(test_mock_web_request_basic_operations);
  RUN_TEST(test_mock_web_request_headers_and_auth);
  RUN_TEST(test_mock_web_response_basic_operations);
  RUN_TEST(test_mock_web_response_headers_and_redirect);
  RUN_TEST(test_string_compatibility);
  RUN_TEST(test_json_serialization);

  UNITY_END();
}

void loop() {}
#endif

// Mock infrastructure test implementations
// (Avoiding separate compilation unit to prevent linking conflicts)

// Helper function for String comparison since Arduino Strings behave
// differently
bool stringContains(const String &str, const char *substring) {
  return str.indexOf(substring) >= 0;
}

// Helper function to create a simple JSON string for testing
String createTestJsonString() {
  DynamicJsonDocument doc(256);
  doc["success"] = true;
  doc["message"] = "Test response";
  doc["count"] = 42;

#ifdef NATIVE_PLATFORM
  std::string result;
  serializeJson(doc, result);
  return String(result.c_str());
#else
  String result;
  serializeJson(doc, result);
  return result;
#endif
}

void setUpMockInfra() {
  // Additional setup for mock infrastructure tests if needed
}

void tearDownMockInfra() {
  // Additional teardown for mock infrastructure tests if needed
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

// Test string compatibility helpers
void test_string_compatibility() {
  // Test that String operations work properly
  String testStr = "Hello, world!";
  TEST_ASSERT_EQUAL(13, testStr.length());
  TEST_ASSERT_TRUE(testStr.indexOf("Hello") == 0);

  // Test substring operations
  String subStr = testStr.substring(0, 5);
  TEST_ASSERT_EQUAL_STRING("Hello", subStr.c_str());
}

// Test JSON string serialization and deserialization
void test_json_serialization() {
  // Create a JSON document
  DynamicJsonDocument doc(256);
  doc["success"] = true;
  doc["message"] = "Test response";
  doc["count"] = 42;

#ifdef NATIVE_PLATFORM
  // Use std::string for native platform
  std::string jsonStr;
  serializeJson(doc, jsonStr);

  // Verify the string was created
  TEST_ASSERT_TRUE(jsonStr.length() > 0);
  TEST_ASSERT_TRUE(jsonStr.find("success") != std::string::npos);
  TEST_ASSERT_TRUE(jsonStr.find("Test response") != std::string::npos);

  // Parse back
  DynamicJsonDocument doc2(256);
  DeserializationError error = deserializeJson(doc2, jsonStr);
#else
  // Use String for Arduino platform
  String jsonStr;
  serializeJson(doc, jsonStr);

  // Verify the string was created
  TEST_ASSERT_TRUE(jsonStr.length() > 0);
  TEST_ASSERT_TRUE(stringContains(jsonStr, "success"));
  TEST_ASSERT_TRUE(stringContains(jsonStr, "Test response"));

  // Parse back
  DynamicJsonDocument doc2(256);
  DeserializationError error = deserializeJson(doc2, jsonStr);
#endif

  // Verify parsing succeeded
  TEST_ASSERT_TRUE(error == DeserializationError::Ok);
  TEST_ASSERT_TRUE(doc2["success"].as<bool>());
  TEST_ASSERT_EQUAL(42, doc2["count"].as<int>());
}
