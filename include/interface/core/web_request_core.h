#ifndef WEB_REQUEST_CORE_H
#define WEB_REQUEST_CORE_H

#include <interface/auth_types.h>
#include <interface/web_module_types.h>
#include <map>
#include <string>

/**
 * WebRequestCore - Pure C++ implementation of HTTP request data
 *
 * Platform-agnostic core logic for handling HTTP request information.
 * Contains only standard C++ types and no Arduino dependencies.
 * Used internally by Arduino WebRequest wrapper.
 */
class WebRequestCore {
private:
  std::string path;
  WebModule::Method method;
  std::string body;
  std::string clientIp;
  std::map<std::string, std::string> params;
  std::map<std::string, std::string> headers;
  std::map<std::string, std::string> jsonParams;
  AuthContext authContext;
  std::string matchedRoutePattern;
  std::string moduleBasePath;

public:
  // Default constructor
  WebRequestCore();

  // Request information getters
  const std::string &getPath() const { return path; }
  WebModule::Method getMethod() const { return method; }
  const std::string &getBody() const { return body; }
  const std::string &getClientIp() const { return clientIp; }

  // Request information setters
  void setPath(const std::string &requestPath) { path = requestPath; }
  void setMethod(WebModule::Method requestMethod) { method = requestMethod; }
  void setBody(const std::string &requestBody) { body = requestBody; }
  void setClientIp(const std::string &ip) { clientIp = ip; }

  // Path parameter helpers
  std::string getRouteParameter(const std::string &paramName) const;

  // URL parameters (query string and POST form data)
  std::string getParam(const std::string &name) const;
  const std::map<std::string, std::string> &getAllParams() const {
    return params;
  }
  void setParam(const std::string &name, const std::string &value) {
    params[name] = value;
  }
  void clearParams() { params.clear(); }

  // Headers
  std::string getHeader(const std::string &name) const;
  void setHeader(const std::string &name, const std::string &value) {
    headers[name] = value;
  }
  void clearHeaders() { headers.clear(); }

  // JSON parameter access
  std::string getJsonParam(const std::string &name) const;
  void setJsonParam(const std::string &name, const std::string &value) {
    jsonParams[name] = value;
  }
  void clearJsonParams() { jsonParams.clear(); }

  // Authentication context
  const AuthContext &getAuthContext() const { return authContext; }
  void setAuthContext(const AuthContext &context) { authContext = context; }

  // Route matching (used by routing system)
  void setMatchedRoute(const std::string &routePattern) {
    matchedRoutePattern = routePattern;
  }
  const std::string &getMatchedRoute() const { return matchedRoutePattern; }

  // Module context (used by template processing)
  void setModuleBasePath(const std::string &basePath) {
    moduleBasePath = basePath;
  }
  const std::string &getModuleBasePath() const { return moduleBasePath; }

  // Content parsing utilities (pure C++)
  void parseQueryParams(const std::string &query);
  void parseFormData(const std::string &formData);
  void parseJsonData(const std::string &jsonData);
  void parseRequestBody(const std::string &body,
                        const std::string &contentType);

private:
  std::string urlDecode(const std::string &str);
  int caseInsensitiveCompare(const std::string &s1,
                             const std::string &s2) const;
};

#endif // WEB_REQUEST_CORE_H