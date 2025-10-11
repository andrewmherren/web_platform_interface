#ifndef MOCK_WEB_PLATFORM_H
#define MOCK_WEB_PLATFORM_H

// Use ArduinoFake for Arduino compatibility
#ifndef NATIVE_PLATFORM
#define NATIVE_PLATFORM 1 // Ensure NATIVE_PLATFORM is defined for testing
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
#include "utils/string_compat.h"

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

// Note: OpenAPIFactory is provided by openapi_factory.h

// Mock request/response classes for testing - simplified without inheritance
class MockWebRequest {
private:
  std::map<std::string, std::string> params;
  std::string body;
  AuthContext authCtx;

public:
  MockWebRequest() {}

  void setParam(const std::string &name, const std::string &value) {
    params[name] = value;
  }

  void setBody(const std::string &b) { body = b; }

  void setAuthContext(bool authenticated, const String &user = "") {
    authCtx.isAuthenticated = authenticated;
    authCtx.username = user;
    if (authenticated) {
      authCtx.authenticatedVia = AuthType::SESSION;
      authCtx.sessionId = "test_session";
    }
  }

  String getParam(const String &name) {
    std::string stdName = name.c_str();
    return params.count(stdName) ? String(params[stdName].c_str()) : String("");
  }

  String getBody() { return String(body.c_str()); }
  const AuthContext &getAuthContext() const { return authCtx; }
};

class MockWebResponse {
public:
  String content;
  String contentType;
  int statusCode = 200;
  std::map<std::string, std::string> headers;

  void setContent(const String &c, const String &ct) {
    content = c;
    contentType = ct;
  }

  void setProgmemContent(const char *c, const String &ct) {
    content = String(c); // In mock, just convert to String
    contentType = ct;
  }

  void setStatus(int code) { statusCode = code; }

  void setHeader(const String &name, const String &value) {
    headers[std::string(name.c_str())] = std::string(value.c_str());
  }
};

// Mock compilation flags for testing
#ifndef OPENAPI_ENABLED
#ifdef WEB_PLATFORM_OPENAPI
#define OPENAPI_ENABLED 1
#else
#define OPENAPI_ENABLED 0
#endif
#endif

#ifndef MAKERAPI_ENABLED
#ifdef WEB_PLATFORM_MAKERAPI
#define MAKERAPI_ENABLED 1
#else
#define MAKERAPI_ENABLED 0
#endif
#endif

#endif // MOCK_WEB_PLATFORM_H