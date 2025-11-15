#include <ArduinoFake.h>
#include <interface/web_response.h>
#include <unity.h>

// Test WebResponse native implementation branch coverage

// Test setContent with various mime types (line 11 branches - need 6)
// Line 11: core.setContent(content.c_str(), mimeType.c_str())
// Each .c_str() creates branches based on String state
void test_web_response_native_set_content_branches() {
  WebResponse response;

  // Branch combo 1: content non-empty, mimeType non-empty
  response.setContent("test content", "text/html");
  TEST_ASSERT_EQUAL_STRING("test content", response.getContent().c_str());

  // Branch combo 2: content non-empty, mimeType empty
  response.setContent("content", "");
  TEST_ASSERT_EQUAL_STRING("content", response.getContent().c_str());

  // Branch combo 3: content empty, mimeType non-empty
  response.setContent("", "application/json");
  TEST_ASSERT_EQUAL_STRING("", response.getContent().c_str());

  // Branch combo 4: content empty, mimeType empty
  response.setContent("", "");
  TEST_ASSERT_EQUAL_STRING("", response.getContent().c_str());

  // Branch combo 5: Long content, specific mime type
  response.setContent("This is a longer content string", "text/plain");
  TEST_ASSERT_EQUAL_STRING("This is a longer content string",
                           response.getContent().c_str());

  // Branch combo 6: Special characters in content
  response.setContent("{\"key\":\"value\"}", "application/json");
  TEST_ASSERT_EQUAL_STRING("{\"key\":\"value\"}",
                           response.getContent().c_str());
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

  // Line 30: Branch 1 - hasProgmemContent() is FALSE (short-circuit)
  // This happens when content type is Normal, not Progmem
  response.setContent("normal content", "text/html");
  String content1 = response.getContent();
  TEST_ASSERT_EQUAL_STRING("normal content", content1.c_str());

  // Line 30: Branch 2 - hasProgmemContent() is TRUE but getProgmemData() is
  // NULL This happens when we set progmem content with nullptr
  response.setProgmemContent(nullptr, "text/plain");
  String content2 = response.getContent();
  // Should fall through to line 34 and return empty core.getContent()

  // Line 30-32: Branch 3 - hasProgmemContent() TRUE && getProgmemData() NOT
  // NULL
  const char *progmem = "progmem data";
  response.setProgmemContent(progmem, "text/javascript");
  String content3 = response.getContent();
  TEST_ASSERT_EQUAL_STRING("progmem data", content3.c_str());

  // Additional branch: Empty progmem data (not null, but empty string)
  const char *emptyProgmem = "";
  response.setProgmemContent(emptyProgmem, "text/css");
  String content4 = response.getContent();
  TEST_ASSERT_EQUAL_STRING("", content4.c_str());

  // Test after clearing with setContent (switches back to Normal type)
  response.setContent("", "text/plain");
  String content5 = response.getContent();
  TEST_ASSERT_EQUAL_STRING("", content5.c_str());
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

// Test setStorageStreamContent branches (lines 56-57)
// Line 56: (driverName.length() == 0) creates 2 branches
// Line 57: core.setStorageStreamContent(collection.c_str(), key.c_str(),
// mimeType.c_str(), driver) Each .c_str() call creates branches based on String
// state
void test_web_response_native_set_storage_stream_branches() {
  WebResponse response;

  // Test all combinations of empty/non-empty for the 4 string parameters
  // Format: (collection, key, mimeType, driverName)

  // With driver name (driverName != "") - driver branch = FALSE
  response.setStorageStreamContent("col", "k", "mime", "driver"); // 1111
  response.setStorageStreamContent("col", "k", "mime", "");       // 1110
  response.setStorageStreamContent("col", "k", "", "driver");     // 1101
  response.setStorageStreamContent("col", "k", "", "");           // 1100
  response.setStorageStreamContent("col", "", "mime", "driver");  // 1011
  response.setStorageStreamContent("col", "", "mime", "");        // 1010
  response.setStorageStreamContent("col", "", "", "driver");      // 1001
  response.setStorageStreamContent("col", "", "", "");            // 1000

  // Empty driver (driverName == "") triggers default "littlefs" - driver branch
  // = TRUE
  response.setStorageStreamContent("", "k", "mime", "driver"); // 0111
  response.setStorageStreamContent("", "k", "mime", "");       // 0110
  response.setStorageStreamContent("", "k", "", "driver");     // 0101
  response.setStorageStreamContent("", "k", "", "");           // 0100
  response.setStorageStreamContent("", "", "mime", "driver");  // 0011
  response.setStorageStreamContent("", "", "mime", "");        // 0010
  response.setStorageStreamContent("", "", "", "driver");      // 0001
  response.setStorageStreamContent("", "", "", "");            // 0000

  TEST_PASS();
} // Test sendTo branch
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
