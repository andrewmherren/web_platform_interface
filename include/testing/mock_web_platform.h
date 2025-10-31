#ifndef MOCK_WEB_PLATFORM_H
#define MOCK_WEB_PLATFORM_H

// Use ArduinoFake for Arduino compatibility
// NATIVE_PLATFORM should be defined by the build system for native testing
// If not defined, assume we're in an Arduino environment
#ifndef NATIVE_PLATFORM
#error                                                                         \
    "NATIVE_PLATFORM must be defined by the build system for native testing. Check your platformio.ini configuration."
#endif

#include "arduino/arduino_string_compat.h"
#include <ArduinoFake.h>
#include <ArduinoJson.h>
#include <functional>
#include <map>
#include <sstream>
#include <string>
#include <vector>

// Use interfaces instead of mocks
#include <interface/auth_types.h>
#include <interface/openapi_types.h>
// Note: platform interface is now in main web_platform_interface.h
#include <interface/utils/route_variant.h>
#include <interface/web_module_interface.h>
#include <interface/web_module_types.h>
#include <interface/web_request.h>
#include <interface/web_response.h>

// Enhanced JsonResponseBuilder with native testing compatibility
class JsonResponseBuilder {
public:
  template <size_t Size>
  static void createResponse(WebResponse &res,
                             std::function<void(JsonObject &)> builder) {
    // Create JSON document for testing
    StaticJsonDocument<Size> doc;
    JsonObject root = doc.template to<JsonObject>();

    // Call the builder function to populate the JSON
    builder(root);

    // Use std::string for native compatibility with ArduinoJson
    std::string jsonString;
    serializeJson(doc, jsonString);

    // Convert to Arduino String for WebResponse
    String arduinoString = String(jsonString.c_str());
    res.setContent(arduinoString, "application/json");
  }
};

// Forward declarations to avoid circular dependencies
class WebServerClass;
struct httpd_req;

// Note: OpenAPIFactory is provided by openapi_factory.h

// Safe mock classes with composition pattern
// These provide compatible interfaces without dangerous casting

class MockWebRequest {
private:
  std::map<std::string, std::string> mockParams;
  String mockBody;
  String mockPath;
  WebModule::Method mockMethod = WebModule::WM_GET;
  AuthContext mockAuthCtx;
  std::map<std::string, std::string> mockHeaders;
  String mockClientIp = "127.0.0.1";
  std::map<std::string, String> mockJsonParams;
  String mockMatchedRoutePattern;
  String mockModuleBasePath;

public:
  // Constructor
  MockWebRequest(const String &path = "/") { mockPath = path; }

  // Test setup methods
  void setParam(const String &name, const String &value) {
    mockParams[std::string(name.c_str())] = std::string(value.c_str());
  }

  void setBody(const String &b) { mockBody = b; }

  void setPath(const String &path) { mockPath = path; }

  void setMethod(WebModule::Method method) { mockMethod = method; }

  void setAuthContext(bool authenticated, const String &user = "") {
    mockAuthCtx.isAuthenticated = authenticated;
    mockAuthCtx.username = user;
    if (authenticated) {
      mockAuthCtx.authenticatedVia = AuthType::SESSION;
      mockAuthCtx.sessionId = "test_session";
    }
  }

  void setMockHeader(const String &name, const String &value) {
    mockHeaders[std::string(name.c_str())] = std::string(value.c_str());
  }

  void setJsonParam(const String &name, const String &value) {
    mockJsonParams[std::string(name.c_str())] = value;
  }

  void setClientIp(const String &ip) { mockClientIp = ip; }

  // WebRequest-compatible interface methods
  String getParam(const String &name) const {
    std::string stdName = name.c_str();
    return mockParams.count(stdName) ? String(mockParams.at(stdName).c_str())
                                     : String("");
  }

  String getBody() const { return mockBody; }

  String getPath() const { return mockPath; }

  WebModule::Method getMethod() const { return mockMethod; }

  const AuthContext &getAuthContext() const { return mockAuthCtx; }

  String getHeader(const String &name) const {
    std::string stdName = name.c_str();
    return mockHeaders.count(stdName) ? String(mockHeaders.at(stdName).c_str())
                                      : String("");
  }

  String getClientIp() const { return mockClientIp; }

  String getJsonParam(const String &name) const {
    std::string stdName = name.c_str();
    return mockJsonParams.count(stdName) ? mockJsonParams.at(stdName)
                                         : String("");
  }

  String getRouteParameter(const String &paramName) const {
    // Mock implementation - could be enhanced for specific tests
    return getParam(paramName);
  }

  std::map<String, String> getAllParams() const {
    std::map<String, String> result;
    for (const auto &pair : mockParams) {
      result[String(pair.first.c_str())] = String(pair.second.c_str());
    }
    return result;
  }

  String getModuleBasePath() const { return mockModuleBasePath; }

  // Additional methods for testing
  void setAuthContext(const AuthContext &context) { mockAuthCtx = context; }
  void setMatchedRoute(const char *routePattern) {
    mockMatchedRoutePattern = routePattern ? String(routePattern) : "";
  }
  void setModuleBasePath(const String &basePath) {
    mockModuleBasePath = basePath;
  }
};

class MockWebResponse {
private:
  String mockContent;
  String mockContentType = "text/html";
  int mockStatusCode = 200;
  std::map<std::string, std::string> mockHeaders;
  bool mockHeadersSent = false;
  bool mockResponseSent = false;

public:
  MockWebResponse() {}

  // WebResponse-compatible interface methods
  void setContent(const String &c, const String &ct = "text/html") {
    mockContent = c;
    mockContentType = ct;
  }

  void setProgmemContent(const char *c, const String &ct) {
    mockContent = String(c); // In mock, just convert to String
    mockContentType = ct;
  }

  void setStatus(int code) { mockStatusCode = code; }

  void setHeader(const String &name, const String &value) {
    mockHeaders[std::string(name.c_str())] = std::string(value.c_str());
  }

  void redirect(const String &url, int code = 302) {
    mockStatusCode = code;
    setHeader("Location", url);
  }

  // Testing accessor methods
  String getContent() const { return mockContent; }

  String getMimeType() const { return mockContentType; }

  String getContentType() const { return mockContentType; }

  String getHeader(const String &name) const {
    std::string stdName = name.c_str();
    return mockHeaders.count(stdName) ? String(mockHeaders.at(stdName).c_str())
                                      : String("");
  }

  int getStatusCode() const { return mockStatusCode; }

  size_t getContentLength() const { return mockContent.length(); }

  bool isHeadersSent() const { return mockHeadersSent; }
  bool isResponseSent() const { return mockResponseSent; }

  // Mock-specific methods for testing
  void markHeadersSent() { mockHeadersSent = true; }
  void markResponseSent() { mockResponseSent = true; }
};

// SAFE ALTERNATIVE: Instead of dangerous casting, use template functions
// that work directly with mock objects. Most tests should use
// MockWebRequest/MockWebResponse directly.

// For code that absolutely needs WebRequest/WebResponse interfaces,
// create wrapper functions that call the methods directly:
template <typename Handler>
auto callWithMockRequest(MockWebRequest &mockReq, Handler handler)
    -> decltype(handler(mockReq)) {
  return handler(mockReq);
}

template <typename Handler>
auto callWithMockResponse(MockWebResponse &mockRes, Handler handler)
    -> decltype(handler(mockRes)) {
  return handler(mockRes);
}

// SAFE ALTERNATIVES TO CASTING - DO NOT USE CASTING AT ALL

// These wrappers allow tests to access the functionality without dangerous
// casts

// For operations that need both request and response
template <typename Function>
void runTestOperation(MockWebRequest &req, MockWebResponse &res,
                      Function operation) {
  operation(req, res);
}

// For request operations
template <typename Function>
void runRequestOperation(MockWebRequest &req, Function operation) {
  operation(req);
}

// For response operations
template <typename Function>
void runResponseOperation(MockWebResponse &res, Function operation) {
  operation(res);
}

// Feature detection for testing - using constexpr for type safety
namespace testing {
// Compile-time feature detection
#ifdef WEB_PLATFORM_OPENAPI
constexpr bool openapi_enabled = (WEB_PLATFORM_OPENAPI != 0);
#else
constexpr bool openapi_enabled = false;
#endif

#ifdef WEB_PLATFORM_MAKERAPI
constexpr bool makerapi_enabled = (WEB_PLATFORM_MAKERAPI != 0);
#else
constexpr bool makerapi_enabled = false;
#endif

// Legacy macro compatibility (for existing code)
#ifndef OPENAPI_ENABLED
#define OPENAPI_ENABLED (testing::openapi_enabled ? 1 : 0)
#endif

#ifndef MAKERAPI_ENABLED
#define MAKERAPI_ENABLED (testing::makerapi_enabled ? 1 : 0)
#endif
} // namespace testing

#endif // MOCK_WEB_PLATFORM_H