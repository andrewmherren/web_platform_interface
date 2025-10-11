#ifndef MOCK_WEB_PLATFORM_H
#define MOCK_WEB_PLATFORM_H

// Use ArduinoFake for Arduino compatibility
#include <ArduinoFake.h>
#include <ArduinoJson.h>
#include <functional>
#include <map>
#include <sstream>
#include <string>
#include <vector>

// Use interfaces instead of mocks
#include <interface/web_module_interface.h>
#include <interface/web_platform_interface.h>
#include <interface/web_module_types.h>
#include <interface/web_request.h>
#include <interface/web_response.h>
#include <interface/auth_types.h>
#include <interface/openapi_types.h>
#include <interface/utils/route_variant.h>

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

// Enhanced OpenAPIFactory
class OpenAPIFactory {
public:
  static OpenAPIDocumentation create(const String &summary,
                                     const String &description,
                                     const String &operationId,
                                     const std::vector<String> &tags) {
    OpenAPIDocumentation doc;
    doc.summary = summary;
    doc.description = description;
    doc.operationId = operationId;
    doc.tags = tags;
    return doc;
  }

  static String createSuccessResponse(const String &description) {
    // Fixed string concatenation issue
    String result = "{\"type\":\"object\",\"description\":\"";
    result += description;
    result += "\"}";
    return result;
  }
};

// Mock request/response classes for testing
class MockWebRequest : public WebRequest {
private:
  std::map<std::string, std::string> params;
  std::string body;
  AuthContext authCtx;

public:
  MockWebRequest() : authCtx(false, "") {}

  void setParam(const std::string &name, const std::string &value) {
    params[name] = value;
  }

  void setBody(const std::string &b) { body = b; }
  void setAuthContext(const AuthContext &ctx) { authCtx = ctx; }

  String getParam(const String &name) override {
    std::string stdName = name.c_str();
    return params.count(stdName) ? String(params[stdName].c_str()) : String("");
  }

  String getBody() override { return String(body.c_str()); }
  const AuthContext &getAuthContext() const override { return authCtx; }
};

class MockWebResponse : public WebResponse {
public:
  String content;
  String contentType;
  int statusCode = 200;
  std::map<std::string, std::string> headers;

  void setContent(const String &c, const String &ct) override {
    content = c;
    contentType = ct;
  }

  void setProgmemContent(const char *c, const String &ct) override {
    content = String(c); // In mock, just convert to String
    contentType = ct;
  }

  void setStatus(int code) override { statusCode = code; }

  void setHeader(const String &name, const String &value) override {
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