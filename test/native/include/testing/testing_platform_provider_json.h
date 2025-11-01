#ifndef TEST_TESTING_PLATFORM_PROVIDER_JSON_H
#define TEST_TESTING_PLATFORM_PROVIDER_JSON_H

#include <unity.h>

// Test functions declarations
void test_create_json_response();
void test_create_json_array_response();
void test_json_responses_with_empty_handlers();
void test_create_complex_json_responses();

// Registration function to run all JSON-related tests
void register_testing_platform_provider_json_tests();

#endif // TEST_TESTING_PLATFORM_PROVIDER_JSON_H