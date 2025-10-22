#ifndef TEST_STRING_COMPAT_H
#define TEST_STRING_COMPAT_H

// Forward declarations for string compatibility tests
void test_string_empty_check();
void test_serialize_json_to_std_string();
void test_deserialize_json_from_std_string();

// Registration function to be called from main
void register_string_compat_tests();

#endif // TEST_STRING_COMPAT_H