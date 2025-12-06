#ifndef WEB_MODULE_INTERFACE_CORE_H
#define WEB_MODULE_INTERFACE_CORE_H

#include <functional>
#include <interface/core/auth_types_core.h>
#include <interface/core/web_request_core.h>
#include <interface/core/web_response_core.h>
#include <string>
#include <vector>

// Forward declarations
struct ApiRouteCore;

namespace WebModuleCore {

// HTTP Methods enum - pure C++ version
enum Method {
  WM_GET = 0,
  WM_POST = 1,
  WM_PUT = 2,
  WM_DELETE = 3,
  WM_PATCH = 4
};

// Core handler types using pure C++ types
typedef std::function<void(WebRequestCore &, WebResponseCore &)>
    UnifiedRouteHandler;

} // namespace WebModuleCore

// Pure C++ web route structure
struct WebRouteCore {
  std::string path;                           // Route path
  WebModuleCore::Method method;               // HTTP method
  WebModuleCore::UnifiedRouteHandler handler; // Unified handler
  std::string contentType;           // Content type (default: "text/html")
  std::string description;           // Human-readable description
  AuthRequirements authRequirements; // Authentication requirements

  // Constructors
  WebRouteCore(const std::string &p, WebModuleCore::Method m,
               WebModuleCore::UnifiedRouteHandler h)
      : path(p), method(m), handler(h), contentType("text/html"),
        authRequirements({AuthType::NONE}) {}

  WebRouteCore(const std::string &p, WebModuleCore::Method m,
               WebModuleCore::UnifiedRouteHandler h, const std::string &ct)
      : path(p), method(m), handler(h), contentType(ct),
        authRequirements({AuthType::NONE}) {}

  WebRouteCore(const std::string &p, WebModuleCore::Method m,
               WebModuleCore::UnifiedRouteHandler h, const std::string &ct,
               const std::string &desc)
      : path(p), method(m), handler(h), contentType(ct), description(desc),
        authRequirements({AuthType::NONE}) {}

  // Constructors with auth requirements
  WebRouteCore(const std::string &p, WebModuleCore::Method m,
               WebModuleCore::UnifiedRouteHandler h,
               const AuthRequirements &auth)
      : path(p), method(m), handler(h), contentType("text/html"),
        authRequirements(auth) {}

  WebRouteCore(const std::string &p, WebModuleCore::Method m,
               WebModuleCore::UnifiedRouteHandler h,
               const AuthRequirements &auth, const std::string &ct)
      : path(p), method(m), handler(h), contentType(ct),
        authRequirements(auth) {}

  WebRouteCore(const std::string &p, WebModuleCore::Method m,
               WebModuleCore::UnifiedRouteHandler h,
               const AuthRequirements &auth, const std::string &ct,
               const std::string &desc)
      : path(p), method(m), handler(h), contentType(ct), description(desc),
        authRequirements(auth) {}
};

// Pure C++ API route structure
struct ApiRouteCore {
  WebRouteCore webRoute; // Route details
  // Note: OpenAPIDocumentation not yet extracted to core, keeping as-is for now

private:
  // Helper function to normalize API paths
  static std::string normalizeApiPath(const std::string &path) {
    // If starts with /api/, remove the /api part
    if (path.substr(0, 5) == "/api/") {
      return path.substr(4); // Remove "/api" keeping the "/"
    }
    // If just "api", return "/"
    if (path == "api") {
      return "/";
    }
    // Otherwise return as-is, ensuring it has a leading slash
    if (!path.empty() && path[0] == '/') {
      return path;
    }
    return "/" + path;
  }

public:
  // Constructors
  ApiRouteCore(const std::string &p, WebModuleCore::Method m,
               WebModuleCore::UnifiedRouteHandler h)
      : webRoute(normalizeApiPath(p), m, h) {}

  ApiRouteCore(const std::string &p, WebModuleCore::Method m,
               WebModuleCore::UnifiedRouteHandler h, const std::string &ct)
      : webRoute(normalizeApiPath(p), m, h, ct) {}

  ApiRouteCore(const std::string &p, WebModuleCore::Method m,
               WebModuleCore::UnifiedRouteHandler h, const std::string &ct,
               const std::string &desc)
      : webRoute(normalizeApiPath(p), m, h, ct, desc) {}

  // Constructors with auth requirements
  ApiRouteCore(const std::string &p, WebModuleCore::Method m,
               WebModuleCore::UnifiedRouteHandler h,
               const AuthRequirements &auth)
      : webRoute(normalizeApiPath(p), m, h, auth) {}

  ApiRouteCore(const std::string &p, WebModuleCore::Method m,
               WebModuleCore::UnifiedRouteHandler h,
               const AuthRequirements &auth, const std::string &ct)
      : webRoute(normalizeApiPath(p), m, h, auth, ct) {}

  ApiRouteCore(const std::string &p, WebModuleCore::Method m,
               WebModuleCore::UnifiedRouteHandler h,
               const AuthRequirements &auth, const std::string &ct,
               const std::string &desc)
      : webRoute(normalizeApiPath(p), m, h, auth, ct, desc) {}
};

// Utility functions for method conversion
inline std::string wmMethodToString(WebModuleCore::Method method) {
  switch (method) {
  case WebModuleCore::WM_GET:
    return "GET";
  case WebModuleCore::WM_POST:
    return "POST";
  case WebModuleCore::WM_PUT:
    return "PUT";
  case WebModuleCore::WM_DELETE:
    return "DELETE";
  case WebModuleCore::WM_PATCH:
    return "PATCH";
  default:
    return "UNKNOWN";
  }
}

#endif // WEB_MODULE_INTERFACE_CORE_H
