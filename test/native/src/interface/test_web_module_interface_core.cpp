#include <interface/core/web_module_interface_core.h>
#include <unity.h>

// Test WebRouteCore constructors
void test_web_route_core_basic_constructor() {
  auto handler = [](WebRequestCore &req, WebResponseCore &res) {};

  WebRouteCore route("/test", WebModuleCore::WM_GET, handler);

  TEST_ASSERT_EQUAL_STRING("/test", route.path.c_str());
  TEST_ASSERT_EQUAL(WebModuleCore::WM_GET, route.method);
  TEST_ASSERT_EQUAL_STRING("text/html", route.contentType.c_str());
  TEST_ASSERT_EQUAL_STRING("", route.description.c_str());
  TEST_ASSERT_EQUAL(1, route.authRequirements.size());
  TEST_ASSERT_EQUAL(AuthType::NONE, route.authRequirements[0]);
}

void test_web_route_core_with_content_type() {
  auto handler = [](WebRequestCore &req, WebResponseCore &res) {};

  WebRouteCore route("/api", WebModuleCore::WM_POST, handler,
                     "application/json");

  TEST_ASSERT_EQUAL_STRING("/api", route.path.c_str());
  TEST_ASSERT_EQUAL(WebModuleCore::WM_POST, route.method);
  TEST_ASSERT_EQUAL_STRING("application/json", route.contentType.c_str());
  TEST_ASSERT_EQUAL_STRING("", route.description.c_str());
  TEST_ASSERT_EQUAL(1, route.authRequirements.size());
  TEST_ASSERT_EQUAL(AuthType::NONE, route.authRequirements[0]);
}

void test_web_route_core_with_description() {
  auto handler = [](WebRequestCore &req, WebResponseCore &res) {};

  WebRouteCore route("/data", WebModuleCore::WM_GET, handler, "text/plain",
                     "Data endpoint");

  TEST_ASSERT_EQUAL_STRING("/data", route.path.c_str());
  TEST_ASSERT_EQUAL(WebModuleCore::WM_GET, route.method);
  TEST_ASSERT_EQUAL_STRING("text/plain", route.contentType.c_str());
  TEST_ASSERT_EQUAL_STRING("Data endpoint", route.description.c_str());
}

void test_web_route_core_with_auth() {
  auto handler = [](WebRequestCore &req, WebResponseCore &res) {};

  WebRouteCore route("/secure", WebModuleCore::WM_DELETE, handler,
                     {AuthType::SESSION});

  TEST_ASSERT_EQUAL_STRING("/secure", route.path.c_str());
  TEST_ASSERT_EQUAL(WebModuleCore::WM_DELETE, route.method);
  TEST_ASSERT_EQUAL_STRING("text/html", route.contentType.c_str());
  TEST_ASSERT_EQUAL(1, route.authRequirements.size());
  TEST_ASSERT_EQUAL(AuthType::SESSION, route.authRequirements[0]);
}

void test_web_route_core_with_auth_and_content_type() {
  auto handler = [](WebRequestCore &req, WebResponseCore &res) {};

  WebRouteCore route("/api/secure", WebModuleCore::WM_PUT, handler,
                     {AuthType::TOKEN}, "application/json");

  TEST_ASSERT_EQUAL_STRING("/api/secure", route.path.c_str());
  TEST_ASSERT_EQUAL(WebModuleCore::WM_PUT, route.method);
  TEST_ASSERT_EQUAL_STRING("application/json", route.contentType.c_str());
  TEST_ASSERT_EQUAL(1, route.authRequirements.size());
  TEST_ASSERT_EQUAL(AuthType::TOKEN, route.authRequirements[0]);
}

void test_web_route_core_with_all_params() {
  auto handler = [](WebRequestCore &req, WebResponseCore &res) {};

  WebRouteCore route("/full", WebModuleCore::WM_PATCH, handler,
                     {AuthType::LOCAL_ONLY}, "text/xml",
                     "Full route with all parameters");

  TEST_ASSERT_EQUAL_STRING("/full", route.path.c_str());
  TEST_ASSERT_EQUAL(WebModuleCore::WM_PATCH, route.method);
  TEST_ASSERT_EQUAL_STRING("text/xml", route.contentType.c_str());
  TEST_ASSERT_EQUAL_STRING("Full route with all parameters",
                           route.description.c_str());
  TEST_ASSERT_EQUAL(1, route.authRequirements.size());
  TEST_ASSERT_EQUAL(AuthType::LOCAL_ONLY, route.authRequirements[0]);
}

// Test ApiRouteCore path normalization
void test_api_route_core_path_normalization() {
  auto handler = [](WebRequestCore &req, WebResponseCore &res) {};

  // Test /api/ prefix removal
  ApiRouteCore route1("/api/test", WebModuleCore::WM_GET, handler);
  TEST_ASSERT_EQUAL_STRING("/test", route1.webRoute.path.c_str());

  // Test /api/endpoint becomes /endpoint
  ApiRouteCore route2("/api/users", WebModuleCore::WM_POST, handler);
  TEST_ASSERT_EQUAL_STRING("/users", route2.webRoute.path.c_str());

  // Test "api" alone becomes "/"
  ApiRouteCore route3("api", WebModuleCore::WM_GET, handler);
  TEST_ASSERT_EQUAL_STRING("/", route3.webRoute.path.c_str());

  // Test path without /api/ gets leading slash added
  ApiRouteCore route4("test", WebModuleCore::WM_GET, handler);
  TEST_ASSERT_EQUAL_STRING("/test", route4.webRoute.path.c_str());

  // Test path with leading slash but no /api/
  ApiRouteCore route5("/test", WebModuleCore::WM_GET, handler);
  TEST_ASSERT_EQUAL_STRING("/test", route5.webRoute.path.c_str());

  // Test /api/v1/endpoint
  ApiRouteCore route6("/api/v1/endpoint", WebModuleCore::WM_GET, handler);
  TEST_ASSERT_EQUAL_STRING("/v1/endpoint", route6.webRoute.path.c_str());
}

// Test ApiRouteCore constructors
void test_api_route_core_basic_constructor() {
  auto handler = [](WebRequestCore &req, WebResponseCore &res) {};

  ApiRouteCore route("/api/test", WebModuleCore::WM_GET, handler);

  TEST_ASSERT_EQUAL_STRING("/test", route.webRoute.path.c_str());
  TEST_ASSERT_EQUAL(WebModuleCore::WM_GET, route.webRoute.method);
  TEST_ASSERT_EQUAL_STRING("text/html", route.webRoute.contentType.c_str());
}

void test_api_route_core_with_content_type() {
  auto handler = [](WebRequestCore &req, WebResponseCore &res) {};

  ApiRouteCore route("/api/data", WebModuleCore::WM_POST, handler,
                     "application/json");

  TEST_ASSERT_EQUAL_STRING("/data", route.webRoute.path.c_str());
  TEST_ASSERT_EQUAL(WebModuleCore::WM_POST, route.webRoute.method);
  TEST_ASSERT_EQUAL_STRING("application/json",
                           route.webRoute.contentType.c_str());
}

void test_api_route_core_with_description() {
  auto handler = [](WebRequestCore &req, WebResponseCore &res) {};

  ApiRouteCore route("/api/endpoint", WebModuleCore::WM_GET, handler,
                     "text/plain", "API endpoint description");

  TEST_ASSERT_EQUAL_STRING("/endpoint", route.webRoute.path.c_str());
  TEST_ASSERT_EQUAL_STRING("text/plain", route.webRoute.contentType.c_str());
  TEST_ASSERT_EQUAL_STRING("API endpoint description",
                           route.webRoute.description.c_str());
}

void test_api_route_core_with_auth() {
  auto handler = [](WebRequestCore &req, WebResponseCore &res) {};

  ApiRouteCore route("/api/secure", WebModuleCore::WM_DELETE, handler,
                     {AuthType::TOKEN});

  TEST_ASSERT_EQUAL_STRING("/secure", route.webRoute.path.c_str());
  TEST_ASSERT_EQUAL(1, route.webRoute.authRequirements.size());
  TEST_ASSERT_EQUAL(AuthType::TOKEN, route.webRoute.authRequirements[0]);
}

void test_api_route_core_with_auth_and_content_type() {
  auto handler = [](WebRequestCore &req, WebResponseCore &res) {};

  ApiRouteCore route("/api/protected", WebModuleCore::WM_PUT, handler,
                     {AuthType::SESSION}, "application/json");

  TEST_ASSERT_EQUAL_STRING("/protected", route.webRoute.path.c_str());
  TEST_ASSERT_EQUAL_STRING("application/json",
                           route.webRoute.contentType.c_str());
  TEST_ASSERT_EQUAL(1, route.webRoute.authRequirements.size());
  TEST_ASSERT_EQUAL(AuthType::SESSION, route.webRoute.authRequirements[0]);
}

void test_api_route_core_with_all_params() {
  auto handler = [](WebRequestCore &req, WebResponseCore &res) {};

  ApiRouteCore route("/api/full", WebModuleCore::WM_PATCH, handler,
                     {AuthType::LOCAL_ONLY}, "application/xml",
                     "Full API route");

  TEST_ASSERT_EQUAL_STRING("/full", route.webRoute.path.c_str());
  TEST_ASSERT_EQUAL(WebModuleCore::WM_PATCH, route.webRoute.method);
  TEST_ASSERT_EQUAL_STRING("application/xml",
                           route.webRoute.contentType.c_str());
  TEST_ASSERT_EQUAL_STRING("Full API route",
                           route.webRoute.description.c_str());
  TEST_ASSERT_EQUAL(1, route.webRoute.authRequirements.size());
  TEST_ASSERT_EQUAL(AuthType::LOCAL_ONLY, route.webRoute.authRequirements[0]);
}

// Test method conversion utility
void test_wm_method_to_string() {
  TEST_ASSERT_EQUAL_STRING("GET",
                           wmMethodToString(WebModuleCore::WM_GET).c_str());
  TEST_ASSERT_EQUAL_STRING("POST",
                           wmMethodToString(WebModuleCore::WM_POST).c_str());
  TEST_ASSERT_EQUAL_STRING("PUT",
                           wmMethodToString(WebModuleCore::WM_PUT).c_str());
  TEST_ASSERT_EQUAL_STRING("DELETE",
                           wmMethodToString(WebModuleCore::WM_DELETE).c_str());
  TEST_ASSERT_EQUAL_STRING("PATCH",
                           wmMethodToString(WebModuleCore::WM_PATCH).c_str());

  // Test with cast to ensure proper handling
  TEST_ASSERT_EQUAL_STRING(
      "UNKNOWN",
      wmMethodToString(static_cast<WebModuleCore::Method>(999)).c_str());
}

// Test multiple auth requirements
void test_web_route_core_multiple_auth() {
  auto handler = [](WebRequestCore &req, WebResponseCore &res) {};

  WebRouteCore route("/multi", WebModuleCore::WM_GET, handler,
                     {AuthType::SESSION, AuthType::TOKEN});

  TEST_ASSERT_EQUAL(2, route.authRequirements.size());
  TEST_ASSERT_EQUAL(AuthType::SESSION, route.authRequirements[0]);
  TEST_ASSERT_EQUAL(AuthType::TOKEN, route.authRequirements[1]);
}

// Test that handler is callable
void test_web_route_core_handler_callable() {
  bool handlerCalled = false;
  auto handler = [&handlerCalled](WebRequestCore &req, WebResponseCore &res) {
    handlerCalled = true;
  };

  WebRouteCore route("/test", WebModuleCore::WM_GET, handler);

  // Create mock request/response
  WebRequestCore req;
  WebResponseCore res;

  // Call the handler
  route.handler(req, res);

  TEST_ASSERT_TRUE(handlerCalled);
}

// Registration function
void register_web_module_interface_core_tests() {
  RUN_TEST(test_web_route_core_basic_constructor);
  RUN_TEST(test_web_route_core_with_content_type);
  RUN_TEST(test_web_route_core_with_description);
  RUN_TEST(test_web_route_core_with_auth);
  RUN_TEST(test_web_route_core_with_auth_and_content_type);
  RUN_TEST(test_web_route_core_with_all_params);
  RUN_TEST(test_api_route_core_path_normalization);
  RUN_TEST(test_api_route_core_basic_constructor);
  RUN_TEST(test_api_route_core_with_content_type);
  RUN_TEST(test_api_route_core_with_description);
  RUN_TEST(test_api_route_core_with_auth);
  RUN_TEST(test_api_route_core_with_auth_and_content_type);
  RUN_TEST(test_api_route_core_with_all_params);
  RUN_TEST(test_wm_method_to_string);
  RUN_TEST(test_web_route_core_multiple_auth);
  RUN_TEST(test_web_route_core_handler_callable);
}
