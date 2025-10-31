#ifndef WEB_MODULE_INTERFACE_H
#define WEB_MODULE_INTERFACE_H

#include <Arduino.h>
#include <functional>
#include <interface/auth_types.h>
#include <interface/core/web_module_interface_core.h>
#include <interface/debug_macros.h>
#include <interface/openapi_factory.h>
#include <interface/openapi_types.h>
#include <interface/utils/route_variant.h>
#include <interface/web_module_types.h>
#include <interface/web_request.h>
#include <interface/web_response.h>
#include <vector>

// Include the unified types definitions
#include <interface/unified_types.h>

// Forward declaration for variant
struct ApiRoute;

// Web route structure - supports both legacy and unified handlers
struct WebRoute {
  String path;                     // Route path (e.g., "/status", "/config")
  WebModule::Method method;        // HTTP method
  WebModule::RouteHandler handler; // Legacy function pointer (deprecated)
  WebModule::UnifiedRouteHandler unifiedHandler; // New unified handler
  String contentType; // Optional: "text/html", "application/json"
  String description; // Optional: Human-readable description
  AuthRequirements authRequirements; // Authentication requirements for route

  // Helper method to convert to core representation
  WebRouteCore toCore() const {
    return WebRouteCore(path.c_str(),
                        static_cast<WebModuleCore::Method>(method),
                        nullptr, // Handler conversion handled separately
                        contentType.c_str(), description.c_str());
  }

private:
  // Helper function to check for API path usage warning
  static void checkApiPathWarning(const String &p) {
    if (p.startsWith("/api/") || p.startsWith("api/")) {
      WARN_PRINTLN(
          "WARNING: WebRoute path '" + p +
          "' starts with '/api/' or 'api/'. Consider using ApiRoute instead "
          "for better API documentation and path normalization.");
    }
  }

public:
  // Constructors for unified handlers
  WebRoute(const String &p, WebModule::Method m,
           WebModule::UnifiedRouteHandler h)
      : path(p), method(m), unifiedHandler(h), contentType("text/html"),
        authRequirements({AuthType::NONE}) {
    checkApiPathWarning(p);
  }

  WebRoute(const String &p, WebModule::Method m,
           WebModule::UnifiedRouteHandler h, const String &ct)
      : path(p), method(m), unifiedHandler(h), contentType(ct),
        authRequirements({AuthType::NONE}) {
    checkApiPathWarning(p);
  }

  WebRoute(const String &p, WebModule::Method m,
           WebModule::UnifiedRouteHandler h, const String &ct,
           const String &desc)
      : path(p), method(m), unifiedHandler(h), contentType(ct),
        description(desc), authRequirements({AuthType::NONE}) {
    checkApiPathWarning(p);
  }

  // Constructors with auth requirements
  WebRoute(const String &p, WebModule::Method m,
           WebModule::UnifiedRouteHandler h, const AuthRequirements &auth)
      : path(p), method(m), unifiedHandler(h), contentType("text/html"),
        authRequirements(auth) {
    checkApiPathWarning(p);
  }

  WebRoute(const String &p, WebModule::Method m,
           WebModule::UnifiedRouteHandler h, const AuthRequirements &auth,
           const String &ct)
      : path(p), method(m), unifiedHandler(h), contentType(ct),
        authRequirements(auth) {
    checkApiPathWarning(p);
  }

  WebRoute(const String &p, WebModule::Method m,
           WebModule::UnifiedRouteHandler h, const AuthRequirements &auth,
           const String &ct, const String &desc)
      : path(p), method(m), unifiedHandler(h), contentType(ct),
        description(desc), authRequirements(auth) {
    checkApiPathWarning(p);
  }
};

struct ApiRoute {
  WebRoute webRoute; // Route details

  OpenAPIDocumentation docs; // OpenAPI documentation

private:
  // Helper function to normalize API paths by removing /api or api prefix
  static String normalizeApiPath(const String &path) {
    // If starts with /api/, remove the /api part
    if (path.startsWith("/api/")) {
      return path.substring(4); // Remove "/api" keeping the "/"
    }
    // If just "api", return "/"
    if (path.equals("api")) {
      return "/";
    }
    // Otherwise return as-is, ensuring it has a leading slash
    if (path.startsWith("/")) {
      return path;
    }
    return "/" + path;
  }

public:
  // Constructors for unified handlers
  ApiRoute(const String &p, WebModule::Method m,
           WebModule::UnifiedRouteHandler h)
      : webRoute(normalizeApiPath(p), m, h) {}

  ApiRoute(const String &p, WebModule::Method m,
           WebModule::UnifiedRouteHandler h, const String &ct)
      : webRoute(normalizeApiPath(p), m, h, ct) {}

  ApiRoute(const String &p, WebModule::Method m,
           WebModule::UnifiedRouteHandler h, const String &ct,
           const String &desc)
      : webRoute(normalizeApiPath(p), m, h, ct, desc) {}

  // Constructors with auth requirements
  ApiRoute(const String &p, WebModule::Method m,
           WebModule::UnifiedRouteHandler h, const AuthRequirements &auth)
      : webRoute(normalizeApiPath(p), m, h, auth) {}

  ApiRoute(const String &p, WebModule::Method m,
           WebModule::UnifiedRouteHandler h, const AuthRequirements &auth,
           const String &ct)
      : webRoute(normalizeApiPath(p), m, h, auth, ct) {}

  ApiRoute(const String &p, WebModule::Method m,
           WebModule::UnifiedRouteHandler h, const AuthRequirements &auth,
           const String &ct, const String &desc)
      : webRoute(normalizeApiPath(p), m, h, auth, ct, desc) {}

  // Constructors with OpenAPI documentation
  ApiRoute(const String &p, WebModule::Method m,
           WebModule::UnifiedRouteHandler h,
           const OpenAPIDocumentation &documentation)
      : webRoute(normalizeApiPath(p), m, h), docs(documentation) {}

  ApiRoute(const String &p, WebModule::Method m,
           WebModule::UnifiedRouteHandler h, const AuthRequirements &auth,
           const OpenAPIDocumentation &documentation)
      : webRoute(normalizeApiPath(p), m, h, auth), docs(documentation) {}

  ApiRoute(const String &p, WebModule::Method m,
           WebModule::UnifiedRouteHandler h, const AuthRequirements &auth,
           const String &ct, const OpenAPIDocumentation &documentation)
      : webRoute(normalizeApiPath(p), m, h, auth, ct), docs(documentation) {}
};

// Abstract interface that all web modules must implement
class IWebModule {
public:
  virtual ~IWebModule() = default;

  // Required methods - pure virtual to enforce implementation
  virtual std::vector<RouteVariant> getHttpRoutes() = 0;
  virtual std::vector<RouteVariant> getHttpsRoutes() = 0;
  virtual String getModuleName() const = 0;

  // Optional implementations with defaults
  virtual String getModuleVersion() const { return "1.0.0"; }
  virtual String getModuleDescription() const { return "Web-enabled module"; }

  // Module lifecycle methods
  virtual void begin() {}
  virtual void begin(const JsonVariant &config) {
    begin();
  } // Default to parameterless begin()
  virtual void handle() {} // Called each loop iteration when in CONNECTED mode

  // Convenience method for modules with identical HTTP/HTTPS routes
  virtual std::vector<RouteVariant> getWebRoutes() { return getHttpRoutes(); }
};

#endif // WEB_MODULE_INTERFACE_H