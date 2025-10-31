#include "../../include/interface/web_module_types.h"
#include <interface/web_module_types.h>
#include <unity.h>

void test_wmMethodToString() {
  TEST_ASSERT_EQUAL_STRING("GET", wmMethodToString(WebModule::WM_GET).c_str());
  TEST_ASSERT_EQUAL_STRING("POST",
                           wmMethodToString(WebModule::WM_POST).c_str());
  TEST_ASSERT_EQUAL_STRING("PUT", wmMethodToString(WebModule::WM_PUT).c_str());
  TEST_ASSERT_EQUAL_STRING("DELETE",
                           wmMethodToString(WebModule::WM_DELETE).c_str());
  TEST_ASSERT_EQUAL_STRING("PATCH",
                           wmMethodToString(WebModule::WM_PATCH).c_str());

  // Test with invalid enum value (casting int to enum)
  TEST_ASSERT_EQUAL_STRING("UNKNOWN",
                           wmMethodToString((WebModule::Method)999).c_str());
}

void test_wmMethodToHttpMethod() {
  TEST_ASSERT_EQUAL(HTTP_GET, wmMethodToHttpMethod(WebModule::WM_GET));
  TEST_ASSERT_EQUAL(HTTP_POST, wmMethodToHttpMethod(WebModule::WM_POST));
  TEST_ASSERT_EQUAL(HTTP_PUT, wmMethodToHttpMethod(WebModule::WM_PUT));
  TEST_ASSERT_EQUAL(HTTP_DELETE, wmMethodToHttpMethod(WebModule::WM_DELETE));
  TEST_ASSERT_EQUAL(HTTP_PATCH, wmMethodToHttpMethod(WebModule::WM_PATCH));

  // Test with invalid enum value - should default to GET
  TEST_ASSERT_EQUAL(HTTP_GET, wmMethodToHttpMethod((WebModule::Method)999));
}

void test_httpMethodToWMMethod() {
  TEST_ASSERT_EQUAL(WebModule::WM_GET, httpMethodToWMMethod(HTTP_GET));
  TEST_ASSERT_EQUAL(WebModule::WM_POST, httpMethodToWMMethod(HTTP_POST));
  TEST_ASSERT_EQUAL(WebModule::WM_PUT, httpMethodToWMMethod(HTTP_PUT));
  TEST_ASSERT_EQUAL(WebModule::WM_DELETE, httpMethodToWMMethod(HTTP_DELETE));
  TEST_ASSERT_EQUAL(WebModule::WM_PATCH, httpMethodToWMMethod(HTTP_PATCH));

  // Test default fallback case (line 67)
  // Use an undefined HTTP method value to test the default case
  const int UNDEFINED_HTTP_METHOD =
      999; // Some value not matching any HTTP_* constant
  TEST_ASSERT_EQUAL(WebModule::WM_GET,
                    httpMethodToWMMethod((HTTPMethod)UNDEFINED_HTTP_METHOD));
}

void test_webmodule_enum_values() {
  // Test that enum values are what we expect (important for serialization)
  TEST_ASSERT_EQUAL(0, WebModule::WM_GET);
  TEST_ASSERT_EQUAL(1, WebModule::WM_POST);
  TEST_ASSERT_EQUAL(2, WebModule::WM_PUT);
  TEST_ASSERT_EQUAL(3, WebModule::WM_DELETE);
  TEST_ASSERT_EQUAL(4, WebModule::WM_PATCH);
}

void register_web_module_types_tests() {
  RUN_TEST(test_wmMethodToString);
  RUN_TEST(test_wmMethodToHttpMethod);
  RUN_TEST(test_httpMethodToWMMethod);
  RUN_TEST(test_webmodule_enum_values);
}
