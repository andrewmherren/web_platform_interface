#ifndef TEST_WEB_REQUEST_H
#define TEST_WEB_REQUEST_H

#include <interface/web_request.h>

// Test function declarations
void test_web_request_path_access();
void test_web_request_method_access();
void test_web_request_body_access();
void test_web_request_query_params();
void test_web_request_headers();
void test_web_request_json_params();
void test_web_request_auth_context();
void test_web_request_route_matching();
void test_web_request_module_base_path();
void test_web_request_client_ip();

// Registration function to be called from main
void register_web_request_tests();

#endif // TEST_WEB_REQUEST_H