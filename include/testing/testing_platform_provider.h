#ifndef TESTING_PLATFORM_PROVIDER_H
#define TESTING_PLATFORM_PROVIDER_H

#include "mock_web_platform.h"
#include <memory>
#include <utility>
#include <vector>
#include <web_platform_interface.h> // Full interface now in main header

/**
 * Single canonical mock platform
 */
class MockWebPlatform : public IWebPlatform {
private:
  String deviceName;
  bool connected = true;
  bool httpsEnabled = true;
  std::vector<std::pair<String, IWebModule *>> registeredModules;
  int routeCount = 0;

  // Callback functions for testing
  std::function<void(const String &)> warnCallback = [](const String &) {};
  std::function<void(const String &)> errorCallback = [](const String &) {};
  std::function<void(const String &)> debugCallback = [](const String &) {};

public:
  void begin(const String &name) override { deviceName = name; }

  void begin(const String &name, bool httpsOnly) override {
    deviceName = name;
    httpsEnabled = httpsOnly;
  }

  void handle() override {
    // Call handle() on all registered modules
    for (const auto &modulePair : registeredModules) {
      IWebModule *module = modulePair.second;
      if (module) {
        module->handle();
      }
    }
  }

  bool isConnected() const override { return connected; }
  bool isHttpsEnabled() const override { return httpsEnabled; }
  String getBaseUrl() const override {
    return httpsEnabled ? "https://mock-device.local"
                        : "http://mock-device.local";
  }

  void registerModule(const String &basePath, IWebModule *module) override {
    registeredModules.push_back(std::make_pair(basePath, module));
    // Add routes from module to mock route count (handle null modules
    // gracefully)
    if (module) {
      auto httpRoutes = module->getHttpRoutes();
      auto httpsRoutes = module->getHttpsRoutes();
      routeCount += httpRoutes.size() + httpsRoutes.size();
    }
  }

  void registerWebRoute(const String &path, PlatformRouteHandler handler,
                        const AuthRequirements &auth,
                        WebModule::Method method) override {
    // Check for API path warning (always enabled for tests)
    if (path.startsWith("/api/") || path.startsWith("api/")) {
      warnCallback(
          "WARNING: registerWebRoute() path '" + path +
          "' starts with '/api/' or 'api/'. Consider using registerApiRoute() "
          "instead for better API documentation and path normalization.");
    }
    routeCount++;
  }

  void registerApiRoute(const String &path, PlatformRouteHandler handler,
                        const AuthRequirements &auth, WebModule::Method method,
                        const OpenAPIDocumentation &docs) override {
    routeCount++;
  }

  size_t getRouteCount() const override { return routeCount; }

  void disableRoute(const String &path, WebModule::Method method) override {
    if (routeCount > 0)
      routeCount--;
  }

  String getDeviceName() const override { return deviceName; }

  void setErrorPage(int statusCode, const String &html) override {
    // Mock implementation
  }

  void addGlobalRedirect(const String &fromPath,
                         const String &toPath) override {
    // Mock implementation
  }

#if defined(NATIVE_PLATFORM)
  // Native builds use core types directly
  void createJsonResponse(WebResponseCore &res,
                          std::function<void(JsonObject &)> builder) override {
    // Create a document and call the builder
    StaticJsonDocument<512> doc;
    JsonObject root = doc.to<JsonObject>();
    builder(root);

    // Serialize to string
    std::string jsonString;
    serializeJson(doc, jsonString);

    // Set content - use std::string directly
    res.setContent(jsonString, "application/json");
  }

  void
  createJsonArrayResponse(WebResponseCore &res,
                          std::function<void(JsonArray &)> builder) override {
    // Create a document and call the builder
    StaticJsonDocument<512> doc;
    JsonArray root = doc.to<JsonArray>();
    builder(root);

    // Serialize to string
    std::string jsonString;
    serializeJson(doc, jsonString);

    // Set content - use std::string directly
    res.setContent(jsonString, "application/json");
  }
#else
  // Arduino builds use wrapper types
  void createJsonResponse(WebResponse &res,
                          std::function<void(JsonObject &)> builder) override {
    // Create a document and call the builder
    StaticJsonDocument<512> doc;
    JsonObject root = doc.to<JsonObject>();
    builder(root);

    // Serialize to string
    std::string jsonString;
    serializeJson(doc, jsonString);

    // Set content - convert to String
    res.setContent(String(jsonString.c_str()), "application/json");
  }

  void
  createJsonArrayResponse(WebResponse &res,
                          std::function<void(JsonArray &)> builder) override {
    // Create a document and call the builder
    StaticJsonDocument<512> doc;
    JsonArray root = doc.to<JsonArray>();
    builder(root);

    // Serialize to string
    std::string jsonString;
    serializeJson(doc, jsonString);

    // Set content - convert to String
    res.setContent(String(jsonString.c_str()), "application/json");
  }
#endif

  // Test utility methods
  void setConnected(bool conn) { connected = conn; }
  int getRegisteredModuleCount() const { return registeredModules.size(); }
  std::vector<std::pair<String, IWebModule *>> getRegisteredModules() const {
    return registeredModules;
  }

  // Callback setters for testing
  void onWarn(std::function<void(const String &)> callback) {
    warnCallback = callback;
  }

  void onError(std::function<void(const String &)> callback) {
    errorCallback = callback;
  }

  void onDebug(std::function<void(const String &)> callback) {
    debugCallback = callback;
  }
};

/**
 * Provider for tests to inject the mock platform
 */
class MockWebPlatformProvider : public IWebPlatformProvider {
private:
  std::unique_ptr<MockWebPlatform> mockPlatform;

public:
  MockWebPlatformProvider()
      : mockPlatform(std::make_unique<MockWebPlatform>()) {}

  IWebPlatform &getPlatform() override { return *mockPlatform; }

  MockWebPlatform &getMockPlatform() { return *mockPlatform; }
};

#endif // TESTING_PLATFORM_PROVIDER_H