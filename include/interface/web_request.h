#ifndef WEB_REQUEST_H
#define WEB_REQUEST_H

// This include is already in Arduino.h, but added here to help linters
#if defined(__CLANG_LINT__)
#include <WString.h>
#endif

#include <Arduino.h>
#include <interface/auth_types.h>
#include <interface/web_module_types.h>
#include <interface/webserver_typedefs.h>
#include <map>

// Common HTTP headers that should be collected by web servers
// Note: Using const char* (non-const array) for WebServer compatibility
extern const char *COMMON_HTTP_HEADERS[];
extern const size_t COMMON_HTTP_HEADERS_COUNT;
struct httpd_req;

/**
 * WebRequest - Unified request abstraction for HTTP/HTTPS handlers
 *
 * Part of the web_module_interface to provide a consistent interface
 * for accessing request data across Arduino WebServer and ESP-IDF
 * HTTP server implementations without modules needing to know about
 * WebPlatform internals.
 */
class WebRequest {
private:
  String path;
  WebModule::Method method;
  String body;
  String clientIp;
  std::map<String, String> params;
  std::map<String, String> headers;
  std::map<String, String> jsonParams;
  AuthContext authContext;    // Authentication information
  String matchedRoutePattern; // Route pattern that matched this request
  String moduleBasePath;      // Base path of the module handling this request

public:
  // Constructor for HTTP server (Arduino WebServer)
  explicit WebRequest(WebServerClass *server);
  explicit WebRequest(httpd_req *req);

  // Request information
  String getPath() const { return path; }
  WebModule::Method getMethod() const { return method; }
  String getBody() const { return body; }
  String getClientIp() const { return clientIp; }

  // Path parameter helpers
  String getRouteParameter(
      const String &paramName) const; // Uses matched route pattern

  // URL parameters (query string and POST form data)
  String getParam(const String &name) const;
  std::map<String, String> getAllParams() const { return params; }

  // Headers
  String getHeader(const String &name) const;

  // JSON parameter access
  String getJsonParam(const String &name) const;

  // Authentication context
  const AuthContext &getAuthContext() const { return authContext; }
  void setAuthContext(const AuthContext &context) { authContext = context; }

  // Route matching (used by routing system)
  void setMatchedRoute(const char *routePattern) {
    matchedRoutePattern = routePattern ? String(routePattern) : "";
  }

  // Module context (used by template processing)
  void setModuleBasePath(const String &basePath) { moduleBasePath = basePath; }
  String getModuleBasePath() const { return moduleBasePath; }

private:
  // Helper method to populate auth context for UI state (not authentication)
  void checkSessionInformation();
  void parseQueryParams(const String &query);
  void parseFormData(const String &formData);
  void parseJsonData(const String &jsonData);
  void parseRequestBody(const String &body, const String &contentType);
  String urlDecode(const String &str);

  void parseClientIp(httpd_req *req);
};

#endif // WEB_REQUEST_H