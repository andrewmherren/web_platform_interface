#include <interface/core/web_response_core.h>
#include <unity.h>

// Test WebResponseCore setMimeType (line 55-57)
void test_web_response_core_mime_type() {
  WebResponseCore response;

  response.setMimeType("application/json");
  TEST_ASSERT_EQUAL_STRING("application/json", response.getMimeType().c_str());

  response.setMimeType("text/html");
  TEST_ASSERT_EQUAL_STRING("text/html", response.getMimeType().c_str());
}

// Test WebResponseCore hasHeader (line 72-73)
void test_web_response_core_has_header() {
  WebResponseCore response;

  // Initially no headers
  TEST_ASSERT_FALSE(response.hasHeader("Content-Type"));

  // Set header
  response.setHeader("Content-Type", "application/json");
  TEST_ASSERT_TRUE(response.hasHeader("Content-Type"));

  // Check non-existent header
  TEST_ASSERT_FALSE(response.hasHeader("X-Custom"));
}

// Test WebResponseCore reset (line 83-95)
void test_web_response_core_reset() {
  WebResponseCore response;

  // Set up response with various properties
  response.setStatus(404);
  response.setContent("Error content");
  response.setMimeType("text/plain");
  response.setHeader("X-Custom", "value");

  // Reset should clear everything
  response.reset();

  TEST_ASSERT_EQUAL(200, response.getStatus());
  TEST_ASSERT_EQUAL_STRING("", response.getContent().c_str());
  TEST_ASSERT_EQUAL_STRING("text/html", response.getMimeType().c_str());
  TEST_ASSERT_FALSE(response.hasHeader("X-Custom"));
  TEST_ASSERT_FALSE(response.isHeadersSent());
  TEST_ASSERT_FALSE(response.isResponseSent());
}

// Test WebResponseCore getHeader
void test_web_response_core_get_header() {
  WebResponseCore response;

  // Get non-existent header
  TEST_ASSERT_EQUAL_STRING("", response.getHeader("Missing").c_str());

  // Set and get header
  response.setHeader("Authorization", "Bearer token");
  TEST_ASSERT_EQUAL_STRING("Bearer token",
                           response.getHeader("Authorization").c_str());
}

// Test WebResponseCore status codes
void test_web_response_core_status_codes() {
  WebResponseCore response;

  // Default status
  TEST_ASSERT_EQUAL(200, response.getStatus());

  // Set various status codes
  response.setStatus(404);
  TEST_ASSERT_EQUAL(404, response.getStatus());

  response.setStatus(500);
  TEST_ASSERT_EQUAL(500, response.getStatus());

  response.setStatus(201);
  TEST_ASSERT_EQUAL(201, response.getStatus());
}

// Test WebResponseCore content operations
void test_web_response_core_content() {
  WebResponseCore response;

  // Set content
  response.setContent("Test content");
  TEST_ASSERT_EQUAL_STRING("Test content", response.getContent().c_str());

  // Clear content (via reset or setContent to empty)
  response.setContent("");
  TEST_ASSERT_EQUAL_STRING("", response.getContent().c_str());
}

// Test WebResponseCore redirect
void test_web_response_core_redirect() {
  WebResponseCore response;

  response.setRedirect("/new-location", 302);
  TEST_ASSERT_EQUAL(302, response.getStatus());
  TEST_ASSERT_EQUAL_STRING("/new-location",
                           response.getHeader("Location").c_str());

  // Test permanent redirect
  response.setRedirect("/permanent", 301);
  TEST_ASSERT_EQUAL(301, response.getStatus());
  TEST_ASSERT_EQUAL_STRING("/permanent",
                           response.getHeader("Location").c_str());
}

// Registration function
void register_web_response_core_tests() {
  RUN_TEST(test_web_response_core_mime_type);
  RUN_TEST(test_web_response_core_has_header);
  RUN_TEST(test_web_response_core_reset);
  RUN_TEST(test_web_response_core_get_header);
  RUN_TEST(test_web_response_core_status_codes);
  RUN_TEST(test_web_response_core_content);
  RUN_TEST(test_web_response_core_redirect);
}
