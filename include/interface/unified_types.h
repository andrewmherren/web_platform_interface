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
}

// Forward declaration for IWebPlatform
class IWebPlatform;

/**
 * Platform provider interface for dependency injection.
 * Modules request a platform instance through this interface.
 */
class IWebPlatformProvider {
public:
    virtual ~IWebPlatformProvider() = default;
    virtual IWebPlatform &getPlatform() = 0;

    // Static instance for global access
    static IWebPlatformProvider *instance;
    static IWebPlatform &getPlatformInstance() {
        if (!instance) {
            // This should never happen in production - platform must be set
            // Use runtime error instead of static_assert for better testing
            // compatibility
            Serial.println("FATAL: WebPlatform provider not initialized");
            while (1); // Halt execution
        }
        return instance->getPlatform();
    }
};

#endif // UNIFIED_TYPES_H