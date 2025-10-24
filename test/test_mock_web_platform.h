#ifndef TEST_MOCK_WEB_PLATFORM_H
#define TEST_MOCK_WEB_PLATFORM_H

// Test basic functionality of the mock platform
void test_mock_web_platform_basics();

// Test route registration and warnings
void test_mock_web_platform_routes();

// Test module registration and handling
void test_mock_web_platform_modules();

// Test JSON response creation
void test_mock_web_platform_json();

// Test callback functions (debug, warn, error)
void test_mock_web_platform_callbacks();

// Test JSON conversion edge cases
void test_mock_web_platform_json_edge_cases();

// Test MockWebPlatformProvider constructor and methods
void test_mock_web_platform_provider();

// Test MockWebRequest constructor and methods
void test_mock_web_request();

// Test MockWebResponse constructor and methods
void test_mock_web_response();

// Test Authentication handling in MockWebRequest
void test_mock_web_request_auth();

// Register all mock platform tests
void register_mock_web_platform_tests();

#endif // TEST_MOCK_WEB_PLATFORM_H