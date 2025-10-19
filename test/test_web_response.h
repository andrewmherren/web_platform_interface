#ifndef TEST_WEB_RESPONSE_H
#define TEST_WEB_RESPONSE_H

// Forward declarations for WebResponse tests
void test_web_response_constructor();
void test_web_response_set_status();
void test_web_response_set_content();
void test_web_response_set_progmem_content();
void test_web_response_set_headers();
void test_web_response_redirect();
void test_web_response_set_json_content();
void test_web_response_send_to();
void test_web_response_storage_stream();
void test_web_response_progmem_data_content();
void test_web_response_send_to_detailed();

// Registration function to be called from main
void register_web_response_tests();

#endif // TEST_WEB_RESPONSE_H