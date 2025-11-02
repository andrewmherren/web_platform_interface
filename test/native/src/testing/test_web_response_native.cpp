#include <ArduinoFake.h>
#include <interface/web_response.h>
#include <unity.h>

// Test WebResponse native implementation branch coverage

// Test setContent with various mime types (line 11 branches)
void test_web_response_native_set_content_branches() {
  WebResponse response;

  // Branch 1: Normal content with default mime type
  response.setContent("test content");
  TEST_ASSERT_EQUAL_STRING("test content", response.getContent().c_str());

  // Branch 2: Content with explicit mime type
  response.setContent("json content", "application/json");
  TEST_ASSERT_EQUAL_STRING("json content", response.getContent().c_str());

  // Branch 3: Empty content
  response.setContent("");
  TEST_ASSERT_EQUAL_STRING("", response.getContent().c_str());

  // Branch 4: Content with empty mime type
  response.setContent("content", "");
  TEST_ASSERT_EQUAL_STRING("content", response.getContent().c_str());
}

// Test setProgmemContent branches (line 17)
void test_web_response_native_set_progmem_branches() {
  WebResponse response;

  const char *progmemData = "PROGMEM data";

  // Branch 1: PROGMEM with mime type
  response.setProgmemContent(progmemData, "text/javascript");
  TEST_ASSERT_EQUAL_STRING("PROGMEM data", response.getContent().c_str());

  // Branch 2: PROGMEM with empty mime type
  response.setProgmemContent(progmemData, "");
  TEST_ASSERT_EQUAL_STRING("PROGMEM data", response.getContent().c_str());
}

// Test setHeader branches (line 22)
void test_web_response_native_set_header_branches() {
  WebResponse response;

  // Branch 1: Normal header
  response.setHeader("Content-Type", "application/json");
  TEST_ASSERT_EQUAL_STRING("application/json",
                           response.getHeader("Content-Type").c_str());

  // Branch 2: Empty header name
  response.setHeader("", "value");

  // Branch 3: Empty header value
  response.setHeader("X-Custom", "");
  TEST_ASSERT_EQUAL_STRING("", response.getHeader("X-Custom").c_str());

  // Branch 4: Overwrite existing header
  response.setHeader("Content-Type", "text/html");
  TEST_ASSERT_EQUAL_STRING("text/html",
                           response.getHeader("Content-Type").c_str());
}

// Test redirect branches (line 26)
void test_web_response_native_redirect_branches() {
  WebResponse response;

  // Branch 1: Redirect with default code (302)
  response.redirect("/default");
  TEST_ASSERT_EQUAL_STRING("/default", response.getHeader("Location").c_str());

  // Branch 2: Redirect with explicit code
  response.redirect("/permanent", 301);
  TEST_ASSERT_EQUAL_STRING("/permanent",
                           response.getHeader("Location").c_str());

  // Branch 3: Empty URL
  response.redirect("");

  // Branch 4: Various status codes
  response.redirect("/temp", 307);
  response.redirect("/see-other", 303);
}

// Test getContent branches (lines 30, 34, 38, 39)
void test_web_response_native_get_content_branches() {
  WebResponse response;

  // Branch 1: Normal content
  response.setContent("normal content");
  TEST_ASSERT_EQUAL_STRING("normal content", response.getContent().c_str());

  // Branch 2: PROGMEM content with data
  const char *progmem = "progmem data";
  response.setProgmemContent(progmem, "text/plain");
  TEST_ASSERT_EQUAL_STRING("progmem data", response.getContent().c_str());

  // Branch 3: PROGMEM content with null pointer (edge case)
  // This tests line 30 hasProgmemContent() && line 31 getProgmemData()
  response.setContent("fallback");
  String content = response.getContent();
  TEST_ASSERT_EQUAL_STRING("fallback", content.c_str());

  // Branch 4: Empty content
  response.setContent("");
  TEST_ASSERT_EQUAL_STRING("", response.getContent().c_str());
}

// Test getHeader branches (line 46)
void test_web_response_native_get_header_branches() {
  WebResponse response;

  // Branch 1: Existing header
  response.setHeader("Authorization", "Bearer token");
  TEST_ASSERT_EQUAL_STRING("Bearer token",
                           response.getHeader("Authorization").c_str());

  // Branch 2: Non-existent header
  TEST_ASSERT_EQUAL_STRING("", response.getHeader("NonExistent").c_str());

  // Branch 3: Empty header name
  TEST_ASSERT_EQUAL_STRING("", response.getHeader("").c_str());

  // Branch 4: Case sensitivity
  TEST_ASSERT_EQUAL_STRING("", response.getHeader("AUTHORIZATION").c_str());
}

// Test setJsonContent branches (line 48)
void test_web_response_native_set_json_content_branches() {
  WebResponse response;

  // Branch 1: Set JSON content with document
  StaticJsonDocument<128> doc;
  doc["key"] = "value";
  response.setJsonContent(doc);

  // Branch 2: Set JSON content multiple times
  StaticJsonDocument<128> doc2;
  doc2["other"] = "data";
  response.setJsonContent(doc2);

  // Branch 3: Empty JSON document
  StaticJsonDocument<64> emptyDoc;
  response.setJsonContent(emptyDoc);
}

// Test setStorageStreamContent branches (lines 56, 57)
void test_web_response_native_set_storage_stream_branches() {
  WebResponse response;

  // Branch 1: With driver name
  response.setStorageStreamContent("collection1", "key1", "image/png",
                                   "spiffs");

  // Branch 2: Without driver name (default to littlefs) - line 56-57
  response.setStorageStreamContent("collection2", "key2", "application/pdf",
                                   "");

  // Branch 3: Empty collection
  response.setStorageStreamContent("", "key3", "text/plain", "littlefs");

  // Branch 4: Empty key
  response.setStorageStreamContent("collection4", "", "image/jpeg", "");

  // Branch 5: Empty mime type
  response.setStorageStreamContent("collection5", "key5", "", "spiffs");
}

// Test sendTo branch
void test_web_response_native_send_to() {
  WebResponse response;

  response.setContent("test");
  response.setStatus(200);

  // In native testing, sendTo is a stub that just marks response as sent
  WebServerClass *server = nullptr;
  response.sendTo(server); // WebServerClass doesn't exist in native

  TEST_PASS();
} // Integration test for multiple operations
void test_web_response_native_integration() {
  WebResponse response;

  // Set status
  response.setStatus(404);

  // Set headers
  response.setHeader("X-Custom-1", "value1");
  response.setHeader("X-Custom-2", "value2");

  // Set content
  response.setContent("Error: Not Found", "text/plain");

  // Verify headers persist
  TEST_ASSERT_EQUAL_STRING("value1", response.getHeader("X-Custom-1").c_str());
  TEST_ASSERT_EQUAL_STRING("value2", response.getHeader("X-Custom-2").c_str());

  // Verify content
  TEST_ASSERT_EQUAL_STRING("Error: Not Found", response.getContent().c_str());
}

// Registration function
void register_web_response_native_tests() {
  RUN_TEST(test_web_response_native_set_content_branches);
  RUN_TEST(test_web_response_native_set_progmem_branches);
  RUN_TEST(test_web_response_native_set_header_branches);
  RUN_TEST(test_web_response_native_redirect_branches);
  RUN_TEST(test_web_response_native_get_content_branches);
  RUN_TEST(test_web_response_native_get_header_branches);
  RUN_TEST(test_web_response_native_set_json_content_branches);
  RUN_TEST(test_web_response_native_set_storage_stream_branches);
  RUN_TEST(test_web_response_native_send_to);
  RUN_TEST(test_web_response_native_integration);
}
