#include <interface/web_request.h>
#include <web_platform_interface.h>

// Common HTTP headers that should be collected by web servers
// Note: Using const char* (non-const array) for WebServer compatibility
const char *COMMON_HTTP_HEADERS[] = {"Host",
                                     "User-Agent",
                                     "Accept",
                                     "Accept-Language",
                                     "Accept-Encoding",
                                     "Content-Type",
                                     "Content-Length",
                                     "Authorization",
                                     "Cookie",
                                     "X-CSRF-Token",
                                     "X-Requested-With",
                                     "Referer",
                                     "Cache-Control",
                                     "Connection",
                                     "Pragma"};
const size_t COMMON_HTTP_HEADERS_COUNT =
    sizeof(COMMON_HTTP_HEADERS) / sizeof(COMMON_HTTP_HEADERS[0]);
