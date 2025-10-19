#ifndef MOCK_WEB_PLATFORM_H
#define MOCK_WEB_PLATFORM_H

// Use ArduinoFake for Arduino compatibility
// NATIVE_PLATFORM should be defined by the build system for native testing
// If not defined, assume we're in an Arduino environment
#ifndef NATIVE_PLATFORM
#error                                                                         \
    "NATIVE_PLATFORM must be defined by the build system for native testing. Check your platformio.ini configuration."
#endif

#include "arduino_string_compat.h"
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

// Include string compatibility helpers
#include "interface/string_compat.h"

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
    std::string jsonString = StringCompat::serializeJsonToStdString(doc);

    // Convert to Arduino String for WebResponse
    String arduinoString = toArduinoString(jsonString);
    res.setContent(arduinoString, "application/json");
  }
};

// Forward declarations to avoid circular dependencies
class WebServerClass;
struct httpd_req;

// Note: OpenAPIFactory is provided by openapi_factory.h

// Enhanced mock classes that provide the same interface as
// WebRequest/WebResponse Using composition pattern to avoid complex inheritance
// issues in testing

class MockWebRequest {
private:
  std::map<std::string, std::string> mockParams;
  std::string mockBody;
  std::string mockPath;
  WebModule::Method mockMethod = WebModule::WM_GET;
  AuthContext mockAuthCtx;
  std::map<std::string, std::string> mockHeaders;
  std::string mockClientIp = "127.0.0.1";

public:
  // Constructor matching the interface needed by tests
  MockWebRequest(const String &path = "/") {
    mockPath = std::string(path.c_str());
  }

  // Test setup methods
  void setParam(const std::string &name, const std::string &value) {
    mockParams[name] = value;
  }

  void setBody(const std::string &b) { mockBody = b; }

  void setPath(const String &path) { mockPath = std::string(path.c_str()); }

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

  // Interface methods matching WebRequest
  String getParam(const String &name) const {
    std::string stdName = name.c_str();
    return mockParams.count(stdName) ? String(mockParams.at(stdName).c_str())
                                     : String("");
  }

  String getBody() const { return String(mockBody.c_str()); }

  String getPath() const { return String(mockPath.c_str()); }

  WebModule::Method getMethod() const { return mockMethod; }

  const AuthContext &getAuthContext() const { return mockAuthCtx; }

  String getHeader(const String &name) const {
    std::string stdName = name.c_str();
    return mockHeaders.count(stdName) ? String(mockHeaders.at(stdName).c_str())
                                      : String("");
  }

  String getClientIp() const { return String(mockClientIp.c_str()); }

  // For modules that need to set auth context
  void setAuthContext(const AuthContext &context) { mockAuthCtx = context; }
};

class MockWebResponse {
private:
  String mockContent;
  String mockContentType = "text/html";
  int mockStatusCode = 200;
  std::map<std::string, std::string> mockHeaders;

public:
  MockWebResponse() {}

  // Interface methods matching WebResponse
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

  String getContentType() const { return mockContentType; }

  size_t getContentLength() const { return mockContent.length(); }

  String getHeader(const String &name) const {
    std::string stdName = name.c_str();
    return mockHeaders.count(stdName) ? String(mockHeaders.at(stdName).c_str())
                                      : String("");
  }

  int getStatusCode() const { return mockStatusCode; }

  String getMimeType() const { return mockContentType; }
};

// Helper casting functions for tests that need WebRequest/WebResponse
// references These use reinterpret_cast which is safe in testing context since
// MockWebRequest/MockWebResponse have compatible memory layouts and interface
// methods
inline WebRequest &asMockWebRequest(MockWebRequest &mockReq) {
  return reinterpret_cast<WebRequest &>(mockReq);
}

inline WebResponse &asMockWebResponse(MockWebResponse &mockRes) {
  return reinterpret_cast<WebResponse &>(mockRes);
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