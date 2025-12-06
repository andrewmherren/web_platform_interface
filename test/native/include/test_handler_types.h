#ifndef TEST_HANDLER_TYPES_H
#define TEST_HANDLER_TYPES_H

// Conditional handler parameter types for tests
// Native tests use core types directly; Arduino tests use wrapper types

#if defined(NATIVE_PLATFORM)
// Native builds use core types
#include <interface/core/web_request_core.h>
#include <interface/core/web_response_core.h>

using TestRequest = WebRequestCore;
using TestResponse = WebResponseCore;

#else
// Arduino/ESP32 builds use wrapper types
#include <interface/web_request.h>
#include <interface/web_response.h>

using TestRequest = WebRequest;
using TestResponse = WebResponse;

#endif

#endif // TEST_HANDLER_TYPES_H
