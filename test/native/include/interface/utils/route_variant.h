#ifndef TEST_ROUTE_VARIANT_H
#define TEST_ROUTE_VARIANT_H

// Forward declarations for route variant tests
void test_route_variant_web_route_constructor();
void test_route_variant_api_route_constructor();
void test_route_variant_copy_constructor();
void test_route_variant_assignment_operator();
void test_route_variant_self_assignment();
void test_route_variant_getters();
void test_route_variant_wrong_type_getters();
void test_route_variant_template_helpers();
void test_web_route_constructors();
void test_api_route_constructors();
void test_api_path_normalization();
void test_web_route_api_path_warning();
void test_openapi_doc_copy_constructor();

// Registration function to be called from main
void register_route_variant_tests();

#endif // TEST_ROUTE_VARIANT_H