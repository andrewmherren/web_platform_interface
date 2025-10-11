#ifndef WEB_PLATFORM_INTERFACE_H
#define WEB_PLATFORM_INTERFACE_H

#include <Arduino.h>
#include <functional>
#include <interface/auth_types.h>
#include <interface/openapi_factory.h>
#include <interface/openapi_types.h>
#include <interface/utils/route_variant.h>
#include <interface/web_module_types.h>
#include <interface/web_request.h>
#include <interface/web_response.h>
#include <vector>

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

// IWebPlatformProvider is defined in unified_types.h
#include <interface/unified_types.h>

#endif // WEB_PLATFORM_INTERFACE_H