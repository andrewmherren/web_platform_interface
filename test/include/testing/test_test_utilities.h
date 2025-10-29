#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

// Interface-specific tests ONLY
// These test the basic functionality that the interface library provides
// NOT intended for use by other modules

void test_string_compatibility();
void test_json_serialization();
void register_helper_tests();

#endif // TEST_HELPERS_H