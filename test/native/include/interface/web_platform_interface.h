#ifndef TEST_WEB_PLATFORM_INTERFACE_H
#define TEST_WEB_PLATFORM_INTERFACE_H

// Forward declarations for web platform interface tests
void test_iwebplatform_lifecycle_operations();
void test_iwebplatform_module_registration();
void test_iwebplatform_route_registration();
void test_iwebplatform_configuration_methods();
void test_iwebplatform_json_response_utilities();
void test_iwebplatformprovider_singleton_pattern();
void test_iwebplatformprovider_dependency_injection();
void test_mock_web_platform_implementation();
void test_platform_provider_error_handling();
void test_integrated_platform_module_workflow();

// Registration function to be called from main
void register_web_platform_interface_tests();

#endif // TEST_WEB_PLATFORM_INTERFACE_H