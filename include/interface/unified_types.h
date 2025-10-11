#ifndef UNIFIED_TYPES_H
#define UNIFIED_TYPES_H

#include <Arduino.h>
#include <functional>
#include <interface/web_request.h>
#include <interface/web_response.h>

// Common type definitions used across multiple interface files
// This helps avoid circular dependencies between interface files

namespace WebModule {
// Route handler function signature (legacy)
typedef std::function<String(const String &requestBody,
                             const std::map<String, String> &params)>
    RouteHandler;

// New unified route handler function signature
typedef std::function<void(WebRequest &, WebResponse &)> UnifiedRouteHandler;
} // namespace WebModule

#endif // UNIFIED_TYPES_H