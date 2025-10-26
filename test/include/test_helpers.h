#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

// Forward declarations for shared helper tests
void test_string_compatibility();
void test_json_serialization();

// Common setup/teardown functions
void setUpCommon();
void tearDownCommon();

// Registration function to be called from main
void register_helper_tests();

#endif // TEST_HELPERS_H