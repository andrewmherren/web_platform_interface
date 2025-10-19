#include "test_core_types.h"
#include <ArduinoJson.h>
#include <unity.h>

// Test the core types and utilities
#include <interface/auth_types.h>
#include <interface/openapi_types.h>
#include <interface/string_compat.h>
#include <interface/web_module_interface.h>
#include <interface/web_module_types.h>

// Test OpenAPIFactory if available
#ifdef OPENAPI_ENABLED
#include <interface/openapi_factory.h>
#endif

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

void test_web_module_types_utilities() {
  // Test wmMethodToString
  TEST_ASSERT_EQUAL_STRING("GET", wmMethodToString(WebModule::WM_GET).c_str());
  TEST_ASSERT_EQUAL_STRING("POST",
                           wmMethodToString(WebModule::WM_POST).c_str());
  TEST_ASSERT_EQUAL_STRING("PUT", wmMethodToString(WebModule::WM_PUT).c_str());
  TEST_ASSERT_EQUAL_STRING("DELETE",
                           wmMethodToString(WebModule::WM_DELETE).c_str());
  TEST_ASSERT_EQUAL_STRING("PATCH",
                           wmMethodToString(WebModule::WM_PATCH).c_str());
  TEST_ASSERT_EQUAL_STRING("UNKNOWN",
                           wmMethodToString((WebModule::Method)99).c_str());

  // Test wmMethodToHttpMethod
  TEST_ASSERT_EQUAL(HTTP_GET, wmMethodToHttpMethod(WebModule::WM_GET));
  TEST_ASSERT_EQUAL(HTTP_POST, wmMethodToHttpMethod(WebModule::WM_POST));
  TEST_ASSERT_EQUAL(HTTP_PUT, wmMethodToHttpMethod(WebModule::WM_PUT));
  TEST_ASSERT_EQUAL(HTTP_DELETE, wmMethodToHttpMethod(WebModule::WM_DELETE));
  TEST_ASSERT_EQUAL(HTTP_PATCH, wmMethodToHttpMethod(WebModule::WM_PATCH));
  TEST_ASSERT_EQUAL(HTTP_GET, wmMethodToHttpMethod(
                                  (WebModule::Method)99)); // Default fallback

  // Test httpMethodToWMMethod
  TEST_ASSERT_EQUAL(WebModule::WM_GET, httpMethodToWMMethod(HTTP_GET));
  TEST_ASSERT_EQUAL(WebModule::WM_POST, httpMethodToWMMethod(HTTP_POST));
  TEST_ASSERT_EQUAL(WebModule::WM_PUT, httpMethodToWMMethod(HTTP_PUT));
  TEST_ASSERT_EQUAL(WebModule::WM_DELETE, httpMethodToWMMethod(HTTP_DELETE));
  TEST_ASSERT_EQUAL(WebModule::WM_PATCH, httpMethodToWMMethod(HTTP_PATCH));
}

void test_string_compat_utilities() {
  // Test StringUtils::isStringEmpty
  String empty("");
  String notEmpty("test");

  TEST_ASSERT_TRUE(StringUtils::isStringEmpty(empty));
  TEST_ASSERT_FALSE(StringUtils::isStringEmpty(notEmpty));

  // Test with zero-length constructed string
  String zeroLength;
  TEST_ASSERT_TRUE(StringUtils::isStringEmpty(zeroLength));

  // Test with whitespace (should not be considered empty)
  String whitespace(" ");
  TEST_ASSERT_FALSE(StringUtils::isStringEmpty(whitespace));
}

// Registration function to run all core type tests
void register_core_types_tests() {
  RUN_TEST(test_web_module_methods);
  RUN_TEST(test_web_module_types_utilities);
  RUN_TEST(test_string_compat_utilities);
  RUN_TEST(test_auth_types);
  RUN_TEST(test_openapi_documentation_basic_operations);

#ifdef OPENAPI_ENABLED
  RUN_TEST(test_openapi_factory_create_documentation);
  RUN_TEST(test_openapi_factory_success_response);
#endif

  RUN_TEST(test_auth_context_basic_operations);
  RUN_TEST(test_auth_requirements_collections);
}
