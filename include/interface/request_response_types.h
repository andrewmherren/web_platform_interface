// Unified request/response type selection for modules
// Use this header in modules instead of including platform-specific headers.

#ifndef INTERFACE_REQUEST_RESPONSE_TYPES_H
#define INTERFACE_REQUEST_RESPONSE_TYPES_H

#if defined(ARDUINO) || defined(ESP_PLATFORM)
#include <interface/web_request.h>
#include <interface/web_response.h>
using RequestT = WebRequest;
using ResponseT = WebResponse;
#else
#include <interface/core/web_request_core.h>
#include <interface/core/web_response_core.h>
using RequestT = WebRequestCore;
using ResponseT = WebResponseCore;
#endif

#endif // INTERFACE_REQUEST_RESPONSE_TYPES_H
