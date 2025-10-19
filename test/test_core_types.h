#ifndef TEST_CORE_TYPES_H
#define TEST_CORE_TYPES_H

// Forward declarations for core type tests
void test_web_module_methods();
void test_web_module_types_utilities();
void test_string_compat_utilities();
void test_auth_types();
void test_openapi_documentation_basic_operations();
void test_auth_context_basic_operations();
void test_auth_requirements_collections();

#ifdef OPENAPI_ENABLED
void test_openapi_factory_create_documentation();
void test_openapi_factory_success_response();
#endif

// Registration function to be called from main
void register_core_types_tests();

#endif // TEST_CORE_TYPES_H