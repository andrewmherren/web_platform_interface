# [Web Platform](https://www.github.com/andrewmherren/web_platform) - Interface and Testing Library

A lightweight interface library that provides core type definitions, interfaces, and testing mocks for the WebPlatform ecosystem. This library enables modular development by defining contracts that modules can depend on without requiring the full WebPlatform implementation, making CI/CD and independent development possible.

## ✨ Features

- 🏗️ **Core Interface Definitions**: Complete IWebModule and IWebPlatform contracts
- 🧪 **Testing Framework**: Mock implementations for unit testing without hardware
- 📦 **Lightweight Dependencies**: Only ArduinoJson dependency for minimal footprint
- 🔧 **CI/CD Friendly**: Enables module testing without full platform dependencies
- 🎯 **Unified Types**: Consistent type definitions across all WebPlatform modules
- 🔗 **Platform Abstraction**: Dependency injection support for different platform implementations

## Core vs Arduino layers

This library exposes a pure C++ core and thin Arduino wrappers:

- Core (header-only types): `include/interface/core/`
    - `web_request_core.h`, `web_response_core.h`, `web_module_interface_core.h`
    - `auth_types_core.h` (AuthType, AuthRequirements) — no Arduino dependencies
- Arduino wrappers: `include/interface/`
    - `web_request.h`, `web_response.h`, `web_module_interface.h`, `auth_types.h`
    - Preserve Arduino `String` APIs and convert to/from the core

OpenAPI documentation types remain Arduino-layer for now; the core intentionally omits doc metadata.

## Purpose

This library serves as the foundational contract layer for the WebPlatform ecosystem, enabling:

### Module Independence
- **Decouple Development**: Modules can be developed and tested independently
- **CI/CD Without Hardware**: Run comprehensive tests without ESP32 hardware
- **Faster Build Times**: Avoid heavy platform dependencies during development

### Consistent Contracts
- **Unified API**: All modules implement the same IWebModule interface
- **Type Safety**: Shared type definitions prevent integration errors
- **Authentication Standards**: Consistent auth patterns across modules

### Testing Support
- **Mock Platform**: Full WebPlatform mock for unit testing
- **Request/Response Mocks**: Simulate HTTP requests and responses
- **Arduino Compatibility**: Native testing with ArduinoFake integration

## Quick Start

### For Module Development

Add the interface library as a dependency:

```ini
# platformio.ini
[env:esp32]
lib_deps = 
    andrewmherren/web_platform_interface@^0.2.0
```

Implement the IWebModule interface:

```cpp
#include <web_platform_interface.h>

class MyModule : public IWebModule {
public:
    std::vector<RouteVariant> getWebRoutes() override {
        return {
            WebRoute("/my-endpoint", WebModule::WM_GET, 
                    [this](WebRequest& req, WebResponse& res) {
                        handleMyEndpoint(req, res);
                    })
        };
    }
    
    void handleMyEndpoint(WebRequest& req, WebResponse& res) {
        res.setContent("Hello from my module!", "text/plain");
    }
    
    // Implement other IWebModule methods...
};
```

### For Testing

Create unit tests using the mock framework:

```cpp
#include <testing/mock_web_platform.h>
#include <testing/testing_platform_provider.h>
#include <unity.h>

void test_module_endpoint() {
    // Set up mock platform provider
    MockWebPlatformProvider provider;
    IWebPlatformProvider::instance = &provider;
    
    // Create mock request/response
    MockWebRequest req("/my-endpoint");
    MockWebResponse res;
    
    // Test your module
    MyModule module;
    module.handleMyEndpoint(req, res);
    
    // Verify results
    TEST_ASSERT_EQUAL_STRING("Hello from my module!", res.getContent().c_str());
    TEST_ASSERT_EQUAL_STRING("text/plain", res.getContentType().c_str());
    
    // Cleanup
    IWebPlatformProvider::instance = nullptr;
}

void register_my_module_tests() {
    RUN_TEST(test_module_endpoint);
}
```

## Core Components

### Interface Definitions

#### IWebModule Interface
The core interface that all WebPlatform modules must implement:

```cpp
class IWebModule {
public:
    virtual ~IWebModule() = default;
    virtual std::vector<RouteVariant> getWebRoutes() = 0;
    virtual std::vector<RouteVariant> getApiRoutes() = 0;
    virtual void onModuleRegistered(const String& basePath, IWebPlatform* platform) {}
    virtual String getModuleName() const { return "Unknown"; }
    virtual String getModuleVersion() const { return "0.0.0"; }
};
```

#### IWebPlatform Interface
Abstract platform interface enabling dependency injection:

```cpp
class IWebPlatform {
public:
    virtual void begin(const String& deviceName) = 0;
    virtual void registerModule(const String& basePath, IWebModule* module) = 0;
    virtual void registerWebRoute(const String& path, 
                                 WebModule::UnifiedRouteHandler handler,
                                 const AuthRequirements& auth = {AuthType::NONE}) = 0;
    // ... other platform methods
};
```

### Type Definitions

#### Authentication Types
```cpp
enum class AuthType {
    NONE,           // Public access
    SESSION,        // Web login required
    TOKEN,          // API token required
    LOCAL_ONLY      // Local network only
};

struct AuthContext {
    bool isAuthenticated = false;
    String username;
    AuthType authenticatedVia = AuthType::NONE;
    String sessionId;
};
```

#### Route Variants
```cpp
struct WebRoute {
    String path;
    WebModule::Method method;
    WebModule::UnifiedRouteHandler handler;
    AuthRequirements auth;
};

struct ApiRoute {
    String path;
    WebModule::Method method;
    WebModule::UnifiedRouteHandler handler;
    AuthRequirements auth;
    OpenAPIDocumentation docs;
};

using RouteVariant = std::variant<WebRoute, ApiRoute>;
```

## Testing Framework

### Mock Web Platform

The `MockWebPlatform` class provides a complete WebPlatform implementation for testing:

```cpp
void test_platform_integration() {
    MockWebPlatform platform;
    MyModule module;
    
    // Register module with mock platform
    platform.registerModule("/my-module", &module);
    
    // Verify registration
    TEST_ASSERT_EQUAL_INT(1, platform.getRegisteredModuleCount());
    
    // Test route resolution
    auto routes = platform.getRegisteredRoutes();
    TEST_ASSERT_TRUE(routes.size() > 0);
}
```

### Mock Request/Response

Realistic mock implementations for comprehensive testing:

```cpp
void test_request_response() {
    MockWebRequest req("/test");
    req.setParam("param1", "value1");
    req.setBody("{\"data\": \"test\"}");
    req.setAuthContext(true, "testuser");
    
    MockWebResponse res;
    
    // Test your handler
    myHandler(req, res);
    
    // Verify response
    TEST_ASSERT_EQUAL_INT(200, res.getStatusCode());
    TEST_ASSERT_EQUAL_STRING("application/json", res.getContentType().c_str());
}
```

### Test Organization Pattern

Follow the library's established pattern:

```cpp
// test/src/test_my_module.cpp
void test_module_basic_functionality() {
    MockWebPlatformProvider provider;
    IWebPlatformProvider::instance = &provider;
    
    MyModule module;
    MockWebRequest req("/test");
    MockWebResponse res;
    
    module.handleTest(req, res);
    
    TEST_ASSERT_EQUAL_INT(200, res.getStatusCode());
    
    IWebPlatformProvider::instance = nullptr;
}

void register_my_module_tests() {
    RUN_TEST(test_module_basic_functionality);
}

// test/test_runner.cpp
#include <ArduinoFake.h>
#include <unity.h>
#include "include/test_my_module.h"

void setUp() { ArduinoFakeReset(); }
void tearDown() {}

#ifdef NATIVE_PLATFORM
int main(int argc, char **argv) {
    UNITY_BEGIN();
    register_my_module_tests();
    return UNITY_END();
}
#else
void setup() {
    UNITY_BEGIN();
    register_my_module_tests();
    UNITY_END();
}
void loop() {}
#endif
```

## CI/CD Integration

Enable comprehensive CI/CD for WebPlatform modules:

```yaml
# .github/workflows/ci.yml
name: CI
on: [push, pull_request]

jobs:
  test:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      - name: Set up PlatformIO
        uses: platformio/platformio-action@v1
      - name: Run tests
        run: |
          pio test -e native
          pio test -e esp32
```

### Test Environment Configuration

```ini
# platformio.ini
[env:native]
platform = native
build_flags = 
    -DNATIVE_PLATFORM=1
    -DUNITY_INCLUDE_CONFIG_H
lib_deps = 
    andrewmherren/web_platform_interface@^0.2.0
    fabiobatsilvestri/ArduinoFake@^0.4.0
test_framework = unity

[env:esp32]
platform = espressif32
board = esp32dev
framework = arduino
lib_deps = 
    andrewmherren/web_platform_interface@^0.2.0
test_framework = unity
```

## Advanced Usage

### Custom Platform Providers

Implement custom platform providers for specialized testing:

```cpp
class CustomTestingProvider : public IWebPlatformProvider {
public:
    CustomTestingProvider(IWebPlatform* platform) : platform_(platform) {}
    IWebPlatform& getPlatform() override { return *platform_; }
private:
    IWebPlatform* platform_;
};
```

### JSON Response Pattern

```cpp
void handleJsonResponse(WebRequest& req, WebResponse& res) {
    auto& platform = IWebPlatformProvider::getPlatformInstance();
    
    platform.createJsonResponse(res, [&](JsonObject& json) {
        json["status"] = "ok";
        json["data"] = "example";
        json["timestamp"] = millis();
    });
}
```

## Memory Considerations

The interface library is designed for minimal memory footprint:

- **Core interfaces**: ~2KB flash, 0KB RAM
- **Mock platform**: ~8KB flash, ~2KB RAM during testing
- **OpenAPI support**: +4KB flash when enabled
- **Testing framework**: ~12KB flash during native testing

### Build Configuration

```ini
# Minimal memory usage
build_flags = 
    -DWEB_PLATFORM_OPENAPI=0    # Disable OpenAPI docs
    -DWEB_PLATFORM_DEBUG=0      # Disable debug output

# Full feature set
build_flags = 
    -DWEB_PLATFORM_OPENAPI=1    # Enable OpenAPI docs
    -DWEB_PLATFORM_MAKERAPI=1   # Enable Maker API filtering
    -DWEB_PLATFORM_DEBUG=1      # Enable debug output
```

## Common Patterns

### Authentication Check Pattern

```cpp
void handleSecureEndpoint(WebRequest& req, WebResponse& res) {
    const auto& auth = req.getAuthContext();
    
    if (!auth.isAuthenticated) {
        res.setStatus(401);
        res.setContent("Authentication required", "text/plain");
        return;
    }
    
    res.setContent("Welcome, " + auth.username, "text/plain");
}
```

## Troubleshooting

### Common Issues

**Compilation errors with String types**: Ensure `#include <testing/arduino_string_compat.h>` for native testing

**Mock platform not found**: Verify `IWebPlatformProvider::instance` is set before using modules

**Route registration failures**: Check that module implements all required IWebModule methods

**Test framework issues**: Ensure `NATIVE_PLATFORM=1` is defined for native testing builds

### Debug Output

Enable interface-level debugging:

```cpp
#define WEB_PLATFORM_INTERFACE_DEBUG 1
#include <web_platform_interface.h>
```

## Contributing

The Web Platform Interface library is the foundation for the entire ecosystem. Changes to this library affect all modules, so:

1. **Interface changes** must maintain backward compatibility
2. **New features** should be optional and feature-flagged
3. **Testing** must cover both Arduino and native environments
4. **Documentation** must be comprehensive and up-to-date

## License

This library is part of the WebPlatform ecosystem and is released under the MIT License.

## Related Projects

- **[web_platform](https://github.com/andrewmherren/web_platform)**: Core WebPlatform implementation
- **[maker_api](https://github.com/andrewmherren/maker_api)**: Interactive API explorer module
- **[usb_pd_controller](https://github.com/andrewmherren/usb_pd_controller)**: USB-C Power Delivery control module
- **[ota_update](https://github.com/andrewmherren/ota_update)**: Over-the-air firmware update module