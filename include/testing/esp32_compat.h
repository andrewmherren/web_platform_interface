#ifndef ESP32_COMPAT_H
#define ESP32_COMPAT_H

/**
 * ESP32 compatibility layer for native tests
 *
 * This file provides mock implementations of ESP32-specific functions,
 * constants, and types that are needed in native tests but not available
 * in the ArduinoFake environment.
 *
 * These are carefully namespaced to avoid collisions with other definitions.
 */

#ifdef NATIVE_PLATFORM

#include <ArduinoFake.h>
#include <cstddef>

// Namespace for ESP32 constants to avoid conflicts with enum values
namespace ESP32Constants {
// ESP error constants
constexpr int ESP_OK = 0;
constexpr int ESP_FAIL = -1;
constexpr int ESP_ERR_NO_MEM = -1;

// HTTP method constants - prefix with ESP32_ to avoid conflicts
constexpr int ESP32_HTTP_GET = 1;
constexpr int ESP32_HTTP_POST = 3;
constexpr int ESP32_HTTP_PUT = 4;
constexpr int ESP32_HTTP_PATCH = 6;
constexpr int ESP32_HTTP_DELETE = 7;

// Socket constants
constexpr int AF_INET = 2;
constexpr int AF_INET6 = 10;
constexpr int INET_ADDRSTRLEN = 16;
constexpr int INET6_ADDRSTRLEN = 46;

// HTTPD constants
constexpr int HTTPD_RESP_USE_STRLEN = -1;
constexpr size_t CONTENT_LENGTH_UNKNOWN = ((size_t)-1);
} // namespace ESP32Constants

// Using declarations for convenience (inside functions only)
using namespace ESP32Constants;

// Extend existing ESPClass with ESP32-specific functions
// (ESPClass is already defined in native_platform_abstraction.h)
extern "C" {
// Add ESP32-specific functions to existing ESPClass
size_t getMaxAllocHeap(); // Forward declaration
}

// Implementation of ESP32-specific functions
inline size_t getMaxAllocHeap() { return 40000; } // Mock value

// Mock ESP-IDF functions that won't work in native tests
inline int httpd_req_get_url_query_str(void *req, char *buf, size_t len) {
  return ESP_FAIL;
}
inline size_t httpd_req_get_hdr_value_len(void *req, const char *field) {
  return 0;
}
inline int httpd_req_get_hdr_value_str(void *req, const char *field, char *val,
                                       size_t len) {
  return ESP_FAIL;
}
inline int httpd_req_recv(void *req, char *buf, size_t len) { return 0; }
inline int httpd_req_to_sockfd(void *req) { return -1; }
inline int httpd_resp_set_type(void *req, const char *type) { return ESP_OK; }
inline int httpd_resp_set_status(void *req, const char *status) {
  return ESP_OK;
}
inline int httpd_resp_send_chunk(void *req, const char *chunk, ssize_t len) {
  return ESP_OK;
}
inline int httpd_resp_send(void *req, const char *buf, ssize_t len) {
  return ESP_OK;
}
inline int getpeername(int sockfd, void *addr, void *addrlen) { return -1; }
inline char *inet_ntop(int af, const void *src, char *dst, unsigned int size) {
  return nullptr;
}
inline unsigned long htonl(unsigned long hostlong) { return hostlong; }

// Mock incomplete struct types
struct httpd_req {
  int method;
  size_t content_len;
};
struct sockaddr_storage {
  int ss_family;
};
struct sockaddr_in {
  void *sin_addr;
};
struct sockaddr_in6 {
  void *sin6_addr;
};
struct in_addr {
  void *s_addr;
};

#endif // NATIVE_PLATFORM

#endif // ESP32_COMPAT_H