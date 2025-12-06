#include <ArduinoFake.h>
#include <interface/web_module_interface.h>
#include <unity.h>

// Test module that uses default implementations
class TestModuleWithDefaults : public IWebModule {
public:
  bool handleCalled = false;
  bool beginCalled = false;
  bool beginWithConfigCalled = false;

  String getModuleName() const override { return "TestModuleWithDefaults"; }

  std::vector<RouteVariant> getHttpRoutes() override {
    return {}; // Empty routes for test
  }

  std::vector<RouteVariant> getHttpsRoutes() override {
    return {}; // Empty routes for test
  }

  // Override to track calls, but still call parent defaults
  void begin() override {
    beginCalled = true;
    IWebModule::begin(); // Call default implementation (line 194)
  }

  void begin(const JsonVariant &config) override {
    beginWithConfigCalled = true;
    IWebModule::begin(config); // Call default implementation (lines 195-197)
  }

  void handle() override {
    handleCalled = true;
    IWebModule::handle(); // Call default implementation (line 198)
  }
};

// Test IWebModule default begin() implementation (line 194)
void test_web_module_default_begin() {
  TestModuleWithDefaults module;

  // Default begin() should do nothing and not crash
  module.begin();
  TEST_ASSERT_TRUE(module.beginCalled);
}

// Test IWebModule default begin(JsonVariant) implementation (lines 195-197)
void test_web_module_default_begin_with_config() {
  TestModuleWithDefaults module;

  // Create a simple JSON config
  StaticJsonDocument<128> doc;
  JsonVariant config = doc.to<JsonVariant>();

  // Default begin(config) should call parameterless begin()
  module.begin(config);
  TEST_ASSERT_TRUE(module.beginWithConfigCalled);
  TEST_ASSERT_TRUE(module.beginCalled); // Should also call begin()
}

// Test IWebModule default handle() implementation (line 198)
void test_web_module_default_handle() {
  TestModuleWithDefaults module;

  // Default handle() should do nothing and not crash
  module.handle();
  TEST_ASSERT_TRUE(module.handleCalled);
}

// Test getWebRoutes() convenience method
void test_web_module_get_web_routes() {
  TestModuleWithDefaults module;

  // getWebRoutes() should return getHttpRoutes() by default
  auto routes = module.getWebRoutes();
  TEST_ASSERT_EQUAL(0, routes.size());
}

// Test that module can be created and used with defaults only
void test_web_module_defaults_integration() {
  TestModuleWithDefaults module;

  // Should be able to call all lifecycle methods without implementing them
  module.begin();
  module.handle();

  TEST_ASSERT_TRUE(module.beginCalled);
  TEST_ASSERT_TRUE(module.handleCalled);

  // Should be able to get routes
  auto httpRoutes = module.getHttpRoutes();
  auto httpsRoutes = module.getHttpsRoutes();
  auto webRoutes = module.getWebRoutes();

  TEST_ASSERT_EQUAL(0, httpRoutes.size());
  TEST_ASSERT_EQUAL(0, httpsRoutes.size());
  TEST_ASSERT_EQUAL(0, webRoutes.size());

  // Should have default version and description
  TEST_ASSERT_EQUAL_STRING("1.0.0", module.getModuleVersion().c_str());
  TEST_ASSERT_EQUAL_STRING("Web-enabled module",
                           module.getModuleDescription().c_str());
}

// Registration function
void register_web_module_lifecycle_tests() {
  RUN_TEST(test_web_module_default_begin);
  RUN_TEST(test_web_module_default_begin_with_config);
  RUN_TEST(test_web_module_default_handle);
  RUN_TEST(test_web_module_get_web_routes);
  RUN_TEST(test_web_module_defaults_integration);
}
