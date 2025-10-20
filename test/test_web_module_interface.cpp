#include "test_web_module_interface.h"
#include <ArduinoFake.h>
#include <ArduinoJson.h>
#include <testing/mock_web_platform.h>
#include <unity.h>
#include <web_platform_interface.h>

// For testing purposes only - avoid affecting global state
#ifdef NATIVE_PLATFORM
// Safe test-local storage
class DebugCapture {
public:
  String output;
  void clear() { output = ""; }
  String get() { return output; }
};

// Use a function-local instance to avoid static initialization issues
DebugCapture &getDebugCapture() {
  static DebugCapture instance;
  return instance;
}
#endif

// Test WebRoute constructors
void test_web_route_full_constructors() {
  // Test basic constructor
  WebRoute route1("/test", WebModule::WM_GET,
                  [](WebRequest &req, WebResponse &res) {
                    res.setContent("Test", "text/plain");
                  });
  TEST_ASSERT_EQUAL_STRING("/test", route1.path.c_str());
  TEST_ASSERT_EQUAL(WebModule::WM_GET, route1.method);
  TEST_ASSERT_EQUAL_STRING("text/html",
                           route1.contentType.c_str()); // Default content type
  TEST_ASSERT_EQUAL_STRING("", route1.description.c_str()); // Empty description
  TEST_ASSERT_EQUAL(1, route1.authRequirements.size());
  TEST_ASSERT_EQUAL(AuthType::NONE, route1.authRequirements[0]);

  // Test constructor with content type
  WebRoute route2(
      "/test2", WebModule::WM_POST,
      [](WebRequest &req, WebResponse &res) {
        res.setContent("{\"test\":true}", "application/json");
      },
      "application/json");
  TEST_ASSERT_EQUAL_STRING("/test2", route2.path.c_str());
  TEST_ASSERT_EQUAL(WebModule::WM_POST, route2.method);
  TEST_ASSERT_EQUAL_STRING("application/json", route2.contentType.c_str());

  // Test constructor with content type and description
  WebRoute route3(
      "/test3", WebModule::WM_PUT,
      [](WebRequest &req, WebResponse &res) {
        res.setContent("Test 3", "text/plain");
      },
      "text/plain", "Test route 3");
  TEST_ASSERT_EQUAL_STRING("/test3", route3.path.c_str());
  TEST_ASSERT_EQUAL(WebModule::WM_PUT, route3.method);
  TEST_ASSERT_EQUAL_STRING("text/plain", route3.contentType.c_str());
  TEST_ASSERT_EQUAL_STRING("Test route 3", route3.description.c_str());

  // Test constructor with auth requirements
  WebRoute route4("/secure", WebModule::WM_GET,
                  [](WebRequest &req, WebResponse &res) {
                    res.setContent("Secure", "text/plain");
                  },
                  {AuthType::SESSION});
  TEST_ASSERT_EQUAL_STRING("/secure", route4.path.c_str());
  TEST_ASSERT_EQUAL(1, route4.authRequirements.size());
  TEST_ASSERT_EQUAL(AuthType::SESSION, route4.authRequirements[0]);

  // Test constructor with auth requirements and content type
  WebRoute route5(
      "/secure2", WebModule::WM_POST,
      [](WebRequest &req, WebResponse &res) {
        res.setContent("Secure 2", "text/plain");
      },
      {AuthType::TOKEN}, "text/plain");
  TEST_ASSERT_EQUAL_STRING("/secure2", route5.path.c_str());
  TEST_ASSERT_EQUAL(WebModule::WM_POST, route5.method);
  TEST_ASSERT_EQUAL_STRING("text/plain", route5.contentType.c_str());
  TEST_ASSERT_EQUAL(1, route5.authRequirements.size());
  TEST_ASSERT_EQUAL(AuthType::TOKEN, route5.authRequirements[0]);

  // Test constructor with auth requirements, content type, and description
  WebRoute route6(
      "/secure3", WebModule::WM_DELETE,
      [](WebRequest &req, WebResponse &res) {
        res.setContent("Secure 3", "text/plain");
      },
      {AuthType::LOCAL_ONLY}, "text/plain", "Secure route 3");
  TEST_ASSERT_EQUAL_STRING("/secure3", route6.path.c_str());
  TEST_ASSERT_EQUAL(WebModule::WM_DELETE, route6.method);
  TEST_ASSERT_EQUAL_STRING("text/plain", route6.contentType.c_str());
  TEST_ASSERT_EQUAL_STRING("Secure route 3", route6.description.c_str());
  TEST_ASSERT_EQUAL(1, route6.authRequirements.size());
  TEST_ASSERT_EQUAL(AuthType::LOCAL_ONLY, route6.authRequirements[0]);
}

// Test ApiRoute OpenAPI documentation
void test_api_route_openapi_docs() {
  // Create OpenAPI documentation
  OpenAPIDocumentation docs;
  // Create docs using builder pattern
  docs =
      OpenAPIDocumentation("Test API", "Test API Description", "testOperation")
          .withResponseSchema(
              R"({"type":"object","properties":{"status":{"type":"string"}}})");

  // Create ApiRoute with OpenAPI docs
  ApiRoute route(
      "/test", WebModule::WM_GET, [](WebRequest &req, WebResponse &res) {},
      docs);

  // Verify docs are stored correctly
  TEST_ASSERT_EQUAL_STRING("Test API", route.docs.getSummary().c_str());
  TEST_ASSERT_EQUAL_STRING("Test API Description",
                           route.docs.getDescription().c_str());
  TEST_ASSERT_EQUAL_STRING("testOperation",
                           route.docs.getOperationId().c_str());
  TEST_ASSERT_TRUE(route.docs.getResponseSchema().indexOf("object") >= 0);

  // Create ApiRoute with auth and OpenAPI docs
  ApiRoute secureRoute(
      "/secure", WebModule::WM_POST, [](WebRequest &req, WebResponse &res) {},
      {AuthType::TOKEN}, docs);

  // Verify both auth and docs are stored correctly
  TEST_ASSERT_EQUAL(1, secureRoute.webRoute.authRequirements.size());
  TEST_ASSERT_EQUAL(AuthType::TOKEN, secureRoute.webRoute.authRequirements[0]);
  TEST_ASSERT_EQUAL_STRING("Test API", secureRoute.docs.getSummary().c_str());

  // Create ApiRoute with auth, content type, and OpenAPI docs
  ApiRoute fullRoute(
      "/full", WebModule::WM_PUT, [](WebRequest &req, WebResponse &res) {},
      {AuthType::SESSION}, "application/json", docs);

  // Verify all components are stored correctly
  TEST_ASSERT_EQUAL_STRING("/full", fullRoute.webRoute.path.c_str());
  TEST_ASSERT_EQUAL(WebModule::WM_PUT, fullRoute.webRoute.method);
  TEST_ASSERT_EQUAL_STRING("application/json",
                           fullRoute.webRoute.contentType.c_str());
  TEST_ASSERT_EQUAL(1, fullRoute.webRoute.authRequirements.size());
  TEST_ASSERT_EQUAL(AuthType::SESSION, fullRoute.webRoute.authRequirements[0]);
  TEST_ASSERT_EQUAL_STRING("Test API", fullRoute.docs.getSummary().c_str());
}

// Test implementation for IWebModule interface testing
class TestWebModuleImpl : public IWebModule {
private:
  String moduleName;
  String moduleVersion;
  String moduleDescription;
  bool beginCalled = false;
  bool configBeginCalled = false;
  bool handleCalled = false;
  JsonVariant lastConfig;

public:
  TestWebModuleImpl(const String &name = "TestModule",
                    const String &version = "1.0.0",
                    const String &description = "Test Module Description")
      : moduleName(name), moduleVersion(version),
        moduleDescription(description) {}

  std::vector<RouteVariant> getHttpRoutes() override {
    return {WebRoute("/module/http", WebModule::WM_GET,
                     [](WebRequest &req, WebResponse &res) {
                       res.setContent("HTTP Route", "text/plain");
                     })};
  }

  std::vector<RouteVariant> getHttpsRoutes() override {
    return {WebRoute("/module/https", WebModule::WM_GET,
                     [](WebRequest &req, WebResponse &res) {
                       res.setContent("HTTPS Route", "text/plain");
                     },
                     {AuthType::SESSION})};
  }

  std::vector<RouteVariant> getWebRoutes() override {
    return {WebRoute("/module/web", WebModule::WM_GET,
                     [](WebRequest &req, WebResponse &res) {
                       res.setContent("Common Route", "text/plain");
                     })};
  }

  String getModuleName() const override { return moduleName; }
  String getModuleVersion() const override { return moduleVersion; }
  String getModuleDescription() const override { return moduleDescription; }

  void begin() override { beginCalled = true; }

  void begin(const JsonVariant &config) override {
    configBeginCalled = true;
    lastConfig = config;
    // Also call base implementation if needed
    if (!beginCalled) {
      begin();
    }
  }

  void handle() override { handleCalled = true; }

  // Test access methods
  bool wasBeginCalled() const { return beginCalled; }
  bool wasConfigBeginCalled() const { return configBeginCalled; }
  bool wasHandleCalled() const { return handleCalled; }
  JsonVariant getLastConfig() const { return lastConfig; }
};

// Test IWebModule interface implementation
void test_i_web_module_interface() {
  TestWebModuleImpl module("CustomModule", "2.0.0", "Custom Description");

  // Test basic attributes
  TEST_ASSERT_EQUAL_STRING("CustomModule", module.getModuleName().c_str());
  TEST_ASSERT_EQUAL_STRING("2.0.0", module.getModuleVersion().c_str());
  TEST_ASSERT_EQUAL_STRING("Custom Description",
                           module.getModuleDescription().c_str());

  // Test route methods
  auto httpRoutes = module.getHttpRoutes();
  TEST_ASSERT_EQUAL(1, httpRoutes.size());
  TEST_ASSERT_TRUE(httpRoutes[0].isWebRoute());
  const WebRoute &httpRoute = httpRoutes[0].getWebRoute();
  TEST_ASSERT_EQUAL_STRING("/module/http", httpRoute.path.c_str());

  auto httpsRoutes = module.getHttpsRoutes();
  TEST_ASSERT_EQUAL(1, httpsRoutes.size());
  TEST_ASSERT_TRUE(httpsRoutes[0].isWebRoute());
  const WebRoute &httpsRoute = httpsRoutes[0].getWebRoute();
  TEST_ASSERT_EQUAL_STRING("/module/https", httpsRoute.path.c_str());
  TEST_ASSERT_EQUAL(1, httpsRoute.authRequirements.size());
  TEST_ASSERT_EQUAL(AuthType::SESSION, httpsRoute.authRequirements[0]);

  auto webRoutes = module.getWebRoutes();
  TEST_ASSERT_EQUAL(1, webRoutes.size());
  TEST_ASSERT_TRUE(webRoutes[0].isWebRoute());
  const WebRoute &webRoute = webRoutes[0].getWebRoute();
  TEST_ASSERT_EQUAL_STRING("/module/web", webRoute.path.c_str());

  // Test default values on a basic implementation
  TestWebModuleImpl defaultModule;
  TEST_ASSERT_EQUAL_STRING("TestModule", defaultModule.getModuleName().c_str());
  TEST_ASSERT_EQUAL_STRING("1.0.0", defaultModule.getModuleVersion().c_str());
  TEST_ASSERT_EQUAL_STRING("Test Module Description",
                           defaultModule.getModuleDescription().c_str());
}

// Test WebModule lifecycle methods
void test_web_module_lifecycle() {
  TestWebModuleImpl module;

  // Verify initial state
  TEST_ASSERT_FALSE(module.wasBeginCalled());
  TEST_ASSERT_FALSE(module.wasConfigBeginCalled());
  TEST_ASSERT_FALSE(module.wasHandleCalled());

  // Test begin()
  module.begin();
  TEST_ASSERT_TRUE(module.wasBeginCalled());
  TEST_ASSERT_FALSE(module.wasConfigBeginCalled());

  // Test handle()
  module.handle();
  TEST_ASSERT_TRUE(module.wasHandleCalled());
}

// Test WebModule with configuration
void test_web_module_with_config() {
  TestWebModuleImpl module;

  // Create config JSON
  StaticJsonDocument<256> configDoc;
  configDoc["enabled"] = true;
  configDoc["interval"] = 1000;
  configDoc["name"] = "Config Test";

  // Test begin with config
  module.begin(configDoc.as<JsonVariant>());

  // Verify config was received and both begin methods were called
  TEST_ASSERT_TRUE(module.wasBeginCalled());
  TEST_ASSERT_TRUE(module.wasConfigBeginCalled());

  // Verify config content
  JsonVariant receivedConfig = module.getLastConfig();
  TEST_ASSERT_TRUE(receivedConfig["enabled"].as<bool>());
  TEST_ASSERT_EQUAL(1000, receivedConfig["interval"].as<int>());
  TEST_ASSERT_EQUAL_STRING("Config Test",
                           receivedConfig["name"].as<const char *>());
}

// Test Auth Requirements in Routes
void test_auth_requirements_in_routes() {
  // Test public route (NONE)
  WebRoute publicRoute("/public", WebModule::WM_GET,
                       [](WebRequest &req, WebResponse &res) {},
                       {AuthType::NONE});
  TEST_ASSERT_EQUAL(1, publicRoute.authRequirements.size());
  TEST_ASSERT_EQUAL(AuthType::NONE, publicRoute.authRequirements[0]);

  // Test session auth route
  WebRoute sessionRoute("/session", WebModule::WM_GET,
                        [](WebRequest &req, WebResponse &res) {},
                        {AuthType::SESSION});
  TEST_ASSERT_EQUAL(1, sessionRoute.authRequirements.size());
  TEST_ASSERT_EQUAL(AuthType::SESSION, sessionRoute.authRequirements[0]);

  // Test token auth route
  WebRoute tokenRoute("/token", WebModule::WM_GET,
                      [](WebRequest &req, WebResponse &res) {},
                      {AuthType::TOKEN});
  TEST_ASSERT_EQUAL(1, tokenRoute.authRequirements.size());
  TEST_ASSERT_EQUAL(AuthType::TOKEN, tokenRoute.authRequirements[0]);

  // Test multiple auth types (ANY of them can access)
  WebRoute multiAuthRoute("/multi-auth", WebModule::WM_GET,
                          [](WebRequest &req, WebResponse &res) {},
                          {AuthType::SESSION, AuthType::TOKEN});
  TEST_ASSERT_EQUAL(2, multiAuthRoute.authRequirements.size());
  TEST_ASSERT_EQUAL(AuthType::SESSION, multiAuthRoute.authRequirements[0]);
  TEST_ASSERT_EQUAL(AuthType::TOKEN, multiAuthRoute.authRequirements[1]);

  // Test local-only route
  WebRoute localRoute("/local", WebModule::WM_GET,
                      [](WebRequest &req, WebResponse &res) {},
                      {AuthType::LOCAL_ONLY});
  TEST_ASSERT_EQUAL(1, localRoute.authRequirements.size());
  TEST_ASSERT_EQUAL(AuthType::LOCAL_ONLY, localRoute.authRequirements[0]);
}

// Test RouteVariant conversions
void test_route_variant_conversions() {
  // Create a WebRoute
  WebRoute webRoute("/web", WebModule::WM_GET,
                    [](WebRequest &req, WebResponse &res) {});

  // Create an ApiRoute
  ApiRoute apiRoute("/api", WebModule::WM_POST,
                    [](WebRequest &req, WebResponse &res) {});

  // Create RouteVariant instances
  RouteVariant webVariant(webRoute);
  RouteVariant apiVariant(apiRoute);

  // Test type checking
  TEST_ASSERT_TRUE(webVariant.isWebRoute());
  TEST_ASSERT_FALSE(webVariant.isApiRoute());

  TEST_ASSERT_FALSE(apiVariant.isWebRoute());
  TEST_ASSERT_TRUE(apiVariant.isApiRoute());

  // Test retrieving the actual routes
  const WebRoute &extractedWebRoute = webVariant.getWebRoute();
  TEST_ASSERT_EQUAL_STRING("/web", extractedWebRoute.path.c_str());
  TEST_ASSERT_EQUAL(WebModule::WM_GET, extractedWebRoute.method);

  const ApiRoute &extractedApiRoute = apiVariant.getApiRoute();
  TEST_ASSERT_EQUAL_STRING("/api", extractedApiRoute.webRoute.path.c_str());
  TEST_ASSERT_EQUAL(WebModule::WM_POST, extractedApiRoute.webRoute.method);

  // Create a collection of mixed routes
  std::vector<RouteVariant> routes = {RouteVariant(webRoute),
                                      RouteVariant(apiRoute)};

  // Test iterating and type checking
  TEST_ASSERT_EQUAL(2, routes.size());
  TEST_ASSERT_TRUE(routes[0].isWebRoute());
  TEST_ASSERT_TRUE(routes[1].isApiRoute());
}

// Registration function
void register_web_module_interface_tests() {
  RUN_TEST(test_web_route_full_constructors);
  RUN_TEST(test_api_route_openapi_docs);
  RUN_TEST(test_i_web_module_interface);
  RUN_TEST(test_web_module_lifecycle);
  RUN_TEST(test_web_module_with_config);
  RUN_TEST(test_auth_requirements_in_routes);
  RUN_TEST(test_route_variant_conversions);
}