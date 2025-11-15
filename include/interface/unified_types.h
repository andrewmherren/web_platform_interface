#ifndef UNIFIED_TYPES_H
#define UNIFIED_TYPES_H

#include "string_compat.h"
#include <functional>
#include <interface/web_request.h>
#include <interface/web_response.h>
#include <map>

#include "core/auth_types_core.h"

// Common type definitions used across multiple interface files
// This helps avoid circular dependencies between interface files

// Legacy Arduino handler type (only for Arduino builds)
#if defined(ARDUINO) || defined(ESP_PLATFORM)
namespace WebModule {
typedef std::function<String(const String &requestBody,
                             const std::map<String, String> &params)>
    RouteHandler;
typedef std::function<void(WebRequest &, WebResponse &)> UnifiedRouteHandler;
} // namespace WebModule
#endif

#endif // UNIFIED_TYPES_H