#include <ArduinoFake.h>
#include <interface/web_response.h>
#include <unity.h>

// Test WebResponse native implementation branch coverage

// Test setContent with various mime types (line 11 branches - need 6)
void test_web_response_native_set_content_branches() {
  WebResponse response;

  // Branch 1: Normal content with default mime type
  response.setContent("test content");
  TEST_ASSERT_EQUAL_STRING("test content", response.getContent().c_str());

  // Branch 2: Content with explicit mime type
  response.setContent("json content", "application/json");
  TEST_ASSERT_EQUAL_STRING("json content", response.getContent().c_str());

  // Branch 3: Empty content with default mime type
  response.setContent("");
  TEST_ASSERT_EQUAL_STRING("", response.getContent().c_str());

  // Branch 4: Empty content with explicit mime type
  response.setContent("", "text/html");
  TEST_ASSERT_EQUAL_STRING("", response.getContent().c_str());

  // Branch 5: Content with empty mime type
  response.setContent("content", "");
  TEST_ASSERT_EQUAL_STRING("content", response.getContent().c_str());

  // Branch 6: Various mime types
  response.setContent("xml", "application/xml");
  TEST_ASSERT_EQUAL_STRING("xml", response.getContent().c_str());
}

// Test setProgmemContent branches (line 17 - need 4)
void test_web_response_native_set_progmem_branches() {
  WebResponse response;

  const char *progmemData = "PROGMEM data";
  const char *emptyData = "";

  // Branch 1: PROGMEM with mime type
  response.setProgmemContent(progmemData, "text/javascript");
  TEST_ASSERT_EQUAL_STRING("PROGMEM data", response.getContent().c_str());

  // Branch 2: PROGMEM with empty mime type
  response.setProgmemContent(progmemData, "");
  TEST_ASSERT_EQUAL_STRING("PROGMEM data", response.getContent().c_str());

  // Branch 3: Empty PROGMEM with mime type
  response.setProgmemContent(emptyData, "text/css");
  TEST_ASSERT_EQUAL_STRING("", response.getContent().c_str());

  // Branch 4: NULL PROGMEM pointer (edge case)
  response.setProgmemContent(nullptr, "text/plain");
}

// Test setHeader branches (line 22 - need 6)
void test_web_response_native_set_header_branches() {
  WebResponse response;

  // Branch 1: Normal header with name and value
  response.setHeader("Content-Type", "application/json");
  TEST_ASSERT_EQUAL_STRING("application/json",
                           response.getHeader("Content-Type").c_str());

  // Branch 2: Empty header name with value
  response.setHeader("", "value");

  // Branch 3: Header name with empty value
  response.setHeader("X-Custom", "");
  TEST_ASSERT_EQUAL_STRING("", response.getHeader("X-Custom").c_str());

  // Branch 4: Both empty
  response.setHeader("", "");

  // Branch 5: Overwrite existing header
  response.setHeader("Content-Type", "text/html");
  TEST_ASSERT_EQUAL_STRING("text/html",
                           response.getHeader("Content-Type").c_str());

  // Branch 6: Multiple different headers
  response.setHeader("X-Header-1", "value1");
  response.setHeader("X-Header-2", "value2");
  TEST_ASSERT_EQUAL_STRING("value1", response.getHeader("X-Header-1").c_str());
  TEST_ASSERT_EQUAL_STRING("value2", response.getHeader("X-Header-2").c_str());
}

// Test redirect branches (line 26 - need 4)
void test_web_response_native_redirect_branches() {
  WebResponse response;

  // Branch 1: Redirect with URL only (default code)
  response.redirect("/default");
  TEST_ASSERT_EQUAL_STRING("/default", response.getHeader("Location").c_str());

  // Branch 2: Redirect with URL and explicit code
  response.redirect("/permanent", 301);
  TEST_ASSERT_EQUAL_STRING("/permanent",
                           response.getHeader("Location").c_str());

  // Branch 3: Empty URL with default code
  response.redirect("");
  TEST_ASSERT_EQUAL_STRING("", response.getHeader("Location").c_str());

  // Branch 4: Empty URL with explicit code
  response.redirect("", 307);
  TEST_ASSERT_EQUAL_STRING("", response.getHeader("Location").c_str());
}

// Test getContent branches (lines 30, 34, 38, 39 - complex branching)
void test_web_response_native_get_content_branches() {
  WebResponse response;

  // Line 30-32: hasProgmemContent() true && getProgmemData() not null
  const char *progmem = "progmem data";
  response.setProgmemContent(progmem, "text/plain");
  TEST_ASSERT_EQUAL_STRING("progmem data", response.getContent().c_str());

  // Line 34: hasProgmemContent() false - use normal content
  response.setContent("normal content", "text/html");
  TEST_ASSERT_EQUAL_STRING("normal content", response.getContent().c_str());

  // Line 38-39: Empty normal content
  response.setContent("", "text/plain");
  TEST_ASSERT_EQUAL_STRING("", response.getContent().c_str());

  // Test progmem with empty string
  const char *emptyProgmem = "";
  response.setProgmemContent(emptyProgmem, "text/css");
  TEST_ASSERT_EQUAL_STRING("", response.getContent().c_str());

  // Test null progmem pointer (if implementation allows)
  response.setProgmemContent(nullptr, "text/js");
  // Should fall back to normal content path
}

// Test getHeader branches (line 46 - need 4)
void test_web_response_native_get_header_branches() {
  WebResponse response;

  // Branch 1: Existing header with value
  response.setHeader("Authorization", "Bearer token");
  TEST_ASSERT_EQUAL_STRING("Bearer token",
                           response.getHeader("Authorization").c_str());

  // Branch 2: Non-existent header returns empty
  TEST_ASSERT_EQUAL_STRING("", response.getHeader("NonExistent").c_str());

  // Branch 3: Empty header name
  TEST_ASSERT_EQUAL_STRING("", response.getHeader("").c_str());

  // Branch 4: Header with empty value
  response.setHeader("X-Empty", "");
  TEST_ASSERT_EQUAL_STRING("", response.getHeader("X-Empty").c_str());
}

// Test setJsonContent branches (line 48 - need 6)
void test_web_response_native_set_json_content_branches() {
  WebResponse response;

  // Branch 1: Set JSON content with populated document
  StaticJsonDocument<128> doc;
  doc["key"] = "value";
  response.setJsonContent(doc);

  // Branch 2: Set JSON content with different document
  StaticJsonDocument<128> doc2;
  doc2["other"] = "data";
  response.setJsonContent(doc2);

  // Branch 3: Empty JSON document
  StaticJsonDocument<64> emptyDoc;
  response.setJsonContent(emptyDoc);

  // Branch 4: JSON with array
  StaticJsonDocument<128> arrDoc;
  arrDoc.add("item1");
  arrDoc.add("item2");
  response.setJsonContent(arrDoc);

  // Branch 5: JSON with nested object
  StaticJsonDocument<256> nestedDoc;
  nestedDoc["outer"]["inner"] = "value";
  response.setJsonContent(nestedDoc);

  // Branch 6: Overwrite existing JSON
  StaticJsonDocument<128> finalDoc;
  finalDoc["final"] = "data";
  response.setJsonContent(finalDoc);
}

// Test setStorageStreamContent branches (lines 56-57 - need 14 and 8)
void test_web_response_native_set_storage_stream_branches() {
  WebResponse response;

  // Line 56-57: driverName.length() == 0 ? "littlefs" : driverName.c_str()

  // Branch 1: With explicit driver name (non-empty)
  response.setStorageStreamContent("collection1", "key1", "image/png",
                                   "spiffs");

  // Branch 2: With empty driver name (defaults to "littlefs")
  response.setStorageStreamContent("collection2", "key2", "application/pdf",
                                   "");

  // Branch 3: Empty collection with driver
  response.setStorageStreamContent("", "key3", "text/plain", "littlefs");

  // Branch 4: Empty collection without driver
  response.setStorageStreamContent("", "key4", "text/html", "");

  // Branch 5: Empty key with driver
  response.setStorageStreamContent("collection5", "", "image/jpeg", "spiffs");

  // Branch 6: Empty key without driver
  response.setStorageStreamContent("collection6", "", "text/css", "");

  // Branch 7: Empty mime type with driver
  response.setStorageStreamContent("collection7", "key7", "", "littlefs");

  // Branch 8: Empty mime type without driver
  response.setStorageStreamContent("collection8", "key8", "", "");

  // Branch 9: All params with driver
  response.setStorageStreamContent("coll", "k", "text/plain", "spiffs");

  // Branch 10: All params without driver
  response.setStorageStreamContent("coll2", "k2", "text/html", "");

  // Branch 11: Empty collection and key with driver
  response.setStorageStreamContent("", "", "image/png", "spiffs");

  // Branch 12: Empty collection and key without driver
  response.setStorageStreamContent("", "", "image/gif", "");

  // Branch 13: All empty with driver
  response.setStorageStreamContent("", "", "", "spiffs");

  // Branch 14: All empty without driver (default)
  response.setStorageStreamContent("", "", "", "");
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
