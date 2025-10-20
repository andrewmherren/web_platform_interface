#ifndef TEST_WEB_MODULE_INTERFACE_H
#define TEST_WEB_MODULE_INTERFACE_H

// Forward declarations for WebModule interface tests
void test_web_route_full_constructors();
void test_api_route_openapi_docs();
void test_i_web_module_interface();
void test_web_module_lifecycle();
void test_web_module_with_config();
void test_auth_requirements_in_routes();
void test_route_variant_conversions();

// Registration function to be called from main
void register_web_module_interface_tests();

#endif // TEST_WEB_MODULE_INTERFACE_H