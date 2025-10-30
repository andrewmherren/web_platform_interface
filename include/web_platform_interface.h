#ifndef WEB_PLATFORM_INTERFACE_H
#define WEB_PLATFORM_INTERFACE_H

// Main include file for web_platform_interface library
// This provides all the core interfaces needed by modules

#include <Arduino.h>
#include <ArduinoJson.h>
#include <functional>
#include <interface/auth_types.h>
#include <interface/openapi_factory.h>
#include <interface/openapi_types.h>
#include <interface/unified_types.h>
#include <interface/utils/route_variant.h>
#include <interface/web_module_interface.h>
#include <interface/web_module_types.h>
#include <interface/web_request.h>
#include <interface/web_response.h>
#include <vector>

// Testing utilities (only include in test builds)
#ifdef UNITY_INCLUDE_CONFIG_H
#include <testing/test_utilities.h>
#endif

// Native platform testing support
#ifdef NATIVE_PLATFORM
#include <testing/native_platform_abstraction.h>
#endif

// Forward declarations
class IWebModule;

/**
 * Abstract interface for web platform implementations.
 * This allows dependency injection of either real WebPlatform or mock
 * implementations.
 */
class IWebPlatform {
public:
  virtual ~IWebPlatform() = default;

  // Core platform lifecycle
  virtual void begin(const String &deviceName) = 0;
  virtual void begin(const String &deviceName, bool httpsOnly) = 0;
  virtual void handle() = 0;

  // Connection state
  virtual bool isConnected() const = 0;
  virtual bool isHttpsEnabled() const = 0;
  virtual String getBaseUrl() const = 0;

  // Module management
  virtual void registerModule(const String &basePath, IWebModule *module) = 0;

  // Route registration - unified API
  virtual void
  registerWebRoute(const String &path, WebModule::UnifiedRouteHandler handler,
                   const AuthRequirements &auth = {AuthType::NONE},
                   WebModule::Method method = WebModule::WM_GET) = 0;

  virtual void registerApiRoute(
      const String &path, WebModule::UnifiedRouteHandler handler,
      const AuthRequirements &auth = {AuthType::NONE},
      WebModule::Method method = WebModule::WM_GET,
      const OpenAPIDocumentation &docs = OpenAPIDocumentation()) = 0;

  // Route management
  virtual size_t getRouteCount() const = 0;
  virtual void disableRoute(const String &path,
                            WebModule::Method method = WebModule::WM_GET) = 0;

  // Configuration and utilities
  virtual String getDeviceName() const = 0;
  virtual void setErrorPage(int statusCode, const String &html) = 0;
  virtual void addGlobalRedirect(const String &fromPath,
                                 const String &toPath) = 0;

  // JSON response utilities
  virtual void
  createJsonResponse(WebResponse &res,
                     std::function<void(JsonObject &)> builder) = 0;

  virtual void
  createJsonArrayResponse(WebResponse &res,
                          std::function<void(JsonArray &)> builder) = 0;
};

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
      while (1)
        ; // Halt execution
    }
    return instance->getPlatform();
  }
};

#endif // WEB_PLATFORM_INTERFACE_H
