#ifndef TEST_WEB_REQUEST_H
#define TEST_WEB_REQUEST_H

// Forward declarations for WebRequest tests
void test_web_request_constants();
void test_web_request_method_enum_coverage();
void test_web_request_auth_context_integration();

// Registration function to be called from main
void register_web_request_tests();

#endif // TEST_WEB_REQUEST_H