#include "../../include/testing/testing_platform_provider_json.h"
#include "native/include/test_handler_types.h"
#include <ArduinoFake.h>
#include <ArduinoJson.h>
#include <testing/testing_platform_provider.h>
#include <unity.h>
#include <web_platform_interface.h>

// Test createJsonResponse method
void test_create_json_response() {
  // Create a MockWebPlatform instance
  MockWebPlatform mockPlatform;

  // Create a response object
  TestResponse response;

  // Test the createJsonResponse method
  mockPlatform.createJsonResponse(response, [](JsonObject &root) {
    root["key1"] = "value1";
    root["key2"] = 42;
    JsonObject nested = root.createNestedObject("nested");
    nested["nestedKey"] = "nestedValue";
  });

  // Verify the response content
  auto content = response.getContent();
  TEST_ASSERT_FALSE(content.empty());
  TEST_ASSERT_EQUAL_STRING("application/json", response.getMimeType().c_str());

  // Parse the JSON to validate its structure
  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, content);
  TEST_ASSERT_FALSE(error);

  // Check JSON values
  TEST_ASSERT_EQUAL_STRING("value1", doc["key1"].as<const char *>());
  TEST_ASSERT_EQUAL(42, doc["key2"].as<int>());
  TEST_ASSERT_EQUAL_STRING("nestedValue",
                           doc["nested"]["nestedKey"].as<const char *>());
}

// Test createJsonArrayResponse method
void test_create_json_array_response() {
  // Create a MockWebPlatform instance
  MockWebPlatform mockPlatform;

  // Create a response object
  TestResponse response;

  // Test the createJsonArrayResponse method
  mockPlatform.createJsonArrayResponse(response, [](JsonArray &array) {
    array.add("item1");
    array.add(42);

    JsonObject obj = array.createNestedObject();
    obj["objKey"] = "objValue";
  });

  // Verify the response content
  auto content = response.getContent();
  TEST_ASSERT_FALSE(content.empty());
  TEST_ASSERT_EQUAL_STRING("application/json", response.getMimeType().c_str());

  // Parse the JSON to validate its structure
  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, content);
  TEST_ASSERT_FALSE(error);

  // Check JSON array values
  TEST_ASSERT_EQUAL_STRING("item1", doc[0].as<const char *>());
  TEST_ASSERT_EQUAL(42, doc[1].as<int>());
  TEST_ASSERT_EQUAL_STRING("objValue", doc[2]["objKey"].as<const char *>());
}

// Test both JSON methods with empty handlers
void test_json_responses_with_empty_handlers() {
  // Create a MockWebPlatform instance
  MockWebPlatform mockPlatform;

  // Test createJsonResponse with empty handler
  {
    TestResponse response;
    mockPlatform.createJsonResponse(response, [](JsonObject &obj) {
      // Empty handler - creates an empty JSON object
    });

    auto content = response.getContent();
    TEST_ASSERT_FALSE(content.empty());
    TEST_ASSERT_EQUAL_STRING("application/json",
                             response.getMimeType().c_str());

    // Verify it's a valid empty JSON object
    TEST_ASSERT_EQUAL_STRING("{}", content.c_str());
  }

  // Test createJsonArrayResponse with empty handler
  {
    TestResponse response;
    mockPlatform.createJsonArrayResponse(response, [](JsonArray &arr) {
      // Empty handler - creates an empty JSON array
    });

    auto content = response.getContent();
    TEST_ASSERT_FALSE(content.empty());
    TEST_ASSERT_EQUAL_STRING("application/json",
                             response.getMimeType().c_str());

    // Verify it's a valid empty JSON array
    TEST_ASSERT_EQUAL_STRING("[]", content.c_str());
  }
}

// Test for complex JSON structures
void test_create_complex_json_responses() {
  MockWebPlatform mockPlatform;

  // Complex JSON object
  {
    TestResponse response;

    mockPlatform.createJsonResponse(response, [](JsonObject &obj) {
      obj["string"] = "text";
      obj["number"] = 123;
      obj["boolean"] = true;

      JsonArray arr = obj.createNestedArray("array");
      arr.add(1);
      arr.add(2);

      JsonObject nested = obj.createNestedObject("object");
      nested["nestedKey"] = "nestedValue";
    });

    auto content = response.getContent();
    TEST_ASSERT_FALSE(content.empty());

    // Verify complex JSON structure
    StaticJsonDocument<512> doc;
    deserializeJson(doc, content);

    TEST_ASSERT_EQUAL_STRING("text", doc["string"].as<const char *>());
    TEST_ASSERT_EQUAL(123, doc["number"].as<int>());
    TEST_ASSERT_EQUAL(true, doc["boolean"].as<bool>());
    TEST_ASSERT_EQUAL(1, doc["array"][0].as<int>());
    TEST_ASSERT_EQUAL(2, doc["array"][1].as<int>());
    TEST_ASSERT_EQUAL_STRING("nestedValue",
                             doc["object"]["nestedKey"].as<const char *>());
  }

  // Complex JSON array
  {
    TestResponse response;

    mockPlatform.createJsonArrayResponse(response, [](JsonArray &arr) {
      arr.add("string");
      arr.add(456);

      JsonObject obj1 = arr.createNestedObject();
      obj1["name"] = "object1";

      JsonArray nestedArr = arr.createNestedArray();
      nestedArr.add(true);
      nestedArr.add("nested");
    });

    auto content = response.getContent();
    TEST_ASSERT_FALSE(content.empty());

    // Verify complex JSON array structure
    StaticJsonDocument<512> doc;
    deserializeJson(doc, content);

    TEST_ASSERT_EQUAL_STRING("string", doc[0].as<const char *>());
    TEST_ASSERT_EQUAL(456, doc[1].as<int>());
    TEST_ASSERT_EQUAL_STRING("object1", doc[2]["name"].as<const char *>());
    TEST_ASSERT_EQUAL(true, doc[3][0].as<bool>());
    TEST_ASSERT_EQUAL_STRING("nested", doc[3][1].as<const char *>());
  }
}

// Registration function to run all JSON-related tests
void register_testing_platform_provider_json_tests() {
  RUN_TEST(test_create_json_response);
  RUN_TEST(test_create_json_array_response);
  RUN_TEST(test_json_responses_with_empty_handlers);
  RUN_TEST(test_create_complex_json_responses);
}
