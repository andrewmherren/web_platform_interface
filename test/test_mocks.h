#ifndef TEST_MOCKS_H
#define TEST_MOCKS_H

// Forward declarations for mock infrastructure tests
void test_mock_platform_module_base_path();
void test_mock_web_request_basic_operations();
void test_mock_web_request_headers_and_auth();
void test_mock_web_response_basic_operations();
void test_mock_web_response_headers_and_redirect();

// Registration function to be called from main
void register_mock_tests();

#endif // TEST_MOCKS_H