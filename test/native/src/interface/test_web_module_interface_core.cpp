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

// Branch coverage tests for constructor initializer lists

// Test WebRouteCore with empty strings (lines 43-44)
void test_web_route_core_empty_strings() {
  auto handler = [](WebRequestCore &req, WebResponseCore &res) {};

  // Empty path
  WebRouteCore route1("", WebModuleCore::WM_GET, handler);
  TEST_ASSERT_EQUAL_STRING("", route1.path.c_str());

  // Empty content type in 2-param constructor (line 48-49)
  WebRouteCore route2("/test", WebModuleCore::WM_POST, handler, "");
  TEST_ASSERT_EQUAL_STRING("", route2.contentType.c_str());

  // Empty content type and description (lines 54-55)
  WebRouteCore route3("/test", WebModuleCore::WM_PUT, handler, "", "");
  TEST_ASSERT_EQUAL_STRING("", route3.contentType.c_str());
  TEST_ASSERT_EQUAL_STRING("", route3.description.c_str());

  // Empty content type with auth (lines 67-68)
  WebRouteCore route4("/test", WebModuleCore::WM_DELETE, handler,
                      {AuthType::SESSION}, "");
  TEST_ASSERT_EQUAL_STRING("", route4.contentType.c_str());

  // Empty everything with auth (lines 75-76)
  WebRouteCore route5("", WebModuleCore::WM_PATCH, handler, {AuthType::TOKEN},
                      "", "");
  TEST_ASSERT_EQUAL_STRING("", route5.path.c_str());
  TEST_ASSERT_EQUAL_STRING("", route5.contentType.c_str());
  TEST_ASSERT_EQUAL_STRING("", route5.description.c_str());
}

// Test ApiRouteCore with empty strings (lines 105, 109, 114, 120, 131, 135)
void test_api_route_core_empty_strings() {
  auto handler = [](WebRequestCore &req, WebResponseCore &res) {};

  // Empty path - should become "/" (line 105)
  ApiRouteCore route1("", WebModuleCore::WM_GET, handler);
  TEST_ASSERT_EQUAL_STRING("/", route1.webRoute.path.c_str());

  // Empty content type (line 109)
  ApiRouteCore route2("/test", WebModuleCore::WM_POST, handler, "");
  TEST_ASSERT_EQUAL_STRING("", route2.webRoute.contentType.c_str());

  // Empty content type and description (line 114)
  ApiRouteCore route3("/test", WebModuleCore::WM_PUT, handler, "", "");
  TEST_ASSERT_EQUAL_STRING("", route3.webRoute.contentType.c_str());
  TEST_ASSERT_EQUAL_STRING("", route3.webRoute.description.c_str());

  // Empty content type with auth (line 131)
  ApiRouteCore route4("/test", WebModuleCore::WM_DELETE, handler,
                      {AuthType::SESSION}, "");
  TEST_ASSERT_EQUAL_STRING("", route4.webRoute.contentType.c_str());

  // Empty everything with auth (line 135)
  ApiRouteCore route5("", WebModuleCore::WM_PATCH, handler, {AuthType::TOKEN},
                      "", "");
  TEST_ASSERT_EQUAL_STRING("/", route5.webRoute.path.c_str());
  TEST_ASSERT_EQUAL_STRING("", route5.webRoute.contentType.c_str());
  TEST_ASSERT_EQUAL_STRING("", route5.webRoute.description.c_str());
}

// Test all HTTP methods (lines 61-62, 87, 92, 95, 138, 140, 142, 144, 146, 148)
void test_all_http_methods() {
  auto handler = [](WebRequestCore &req, WebResponseCore &res) {};

  // Test all methods with basic constructor
  WebRouteCore getRoute("/get", WebModuleCore::WM_GET, handler);
  TEST_ASSERT_EQUAL(WebModuleCore::WM_GET, getRoute.method);

  WebRouteCore postRoute("/post", WebModuleCore::WM_POST, handler);
  TEST_ASSERT_EQUAL(WebModuleCore::WM_POST, postRoute.method);

  WebRouteCore putRoute("/put", WebModuleCore::WM_PUT, handler);
  TEST_ASSERT_EQUAL(WebModuleCore::WM_PUT, putRoute.method);

  WebRouteCore deleteRoute("/delete", WebModuleCore::WM_DELETE, handler);
  TEST_ASSERT_EQUAL(WebModuleCore::WM_DELETE, deleteRoute.method);

  WebRouteCore patchRoute("/patch", WebModuleCore::WM_PATCH, handler);
  TEST_ASSERT_EQUAL(WebModuleCore::WM_PATCH, patchRoute.method);

  // Test with auth constructors to hit lines 61-62
  WebRouteCore authGetRoute("/auth", WebModuleCore::WM_GET, handler,
                            {AuthType::NONE});
  TEST_ASSERT_EQUAL(WebModuleCore::WM_GET, authGetRoute.method);

  WebRouteCore authPostRoute("/auth", WebModuleCore::WM_POST, handler,
                             {AuthType::SESSION});
  TEST_ASSERT_EQUAL(WebModuleCore::WM_POST, authPostRoute.method);

  // Test with content type constructors
  WebRouteCore ctRoute("/ct", WebModuleCore::WM_PUT, handler, "text/plain");
  TEST_ASSERT_EQUAL(WebModuleCore::WM_PUT, ctRoute.method);

  // Test with description constructors
  WebRouteCore descRoute("/desc", WebModuleCore::WM_DELETE, handler, "text/xml",
                         "description");
  TEST_ASSERT_EQUAL(WebModuleCore::WM_DELETE, descRoute.method);
}

// Test various auth requirement combinations
void test_auth_requirements_variations() {
  auto handler = [](WebRequestCore &req, WebResponseCore &res) {};

  // Empty auth requirements - use explicit cast to avoid ambiguity
  AuthRequirements emptyAuth;
  WebRouteCore route1("/test", WebModuleCore::WM_GET, handler, emptyAuth);
  TEST_ASSERT_EQUAL(0, route1.authRequirements.size());

  // Single auth
  WebRouteCore route2("/test", WebModuleCore::WM_POST, handler,
                      {AuthType::TOKEN});
  TEST_ASSERT_EQUAL(1, route2.authRequirements.size());

  // Multiple auth
  WebRouteCore route3(
      "/test", WebModuleCore::WM_PUT, handler,
      {AuthType::SESSION, AuthType::TOKEN, AuthType::LOCAL_ONLY});
  TEST_ASSERT_EQUAL(3, route3.authRequirements.size());

  // Auth with content type
  WebRouteCore route4("/test", WebModuleCore::WM_DELETE, handler,
                      {AuthType::NONE}, "application/json");
  TEST_ASSERT_EQUAL(1, route4.authRequirements.size());
  TEST_ASSERT_EQUAL_STRING("application/json", route4.contentType.c_str());

  // Auth with full params
  WebRouteCore route5("/test", WebModuleCore::WM_PATCH, handler,
                      {AuthType::SESSION, AuthType::TOKEN}, "text/html",
                      "Secure endpoint");
  TEST_ASSERT_EQUAL(2, route5.authRequirements.size());
  TEST_ASSERT_EQUAL_STRING("text/html", route5.contentType.c_str());
  TEST_ASSERT_EQUAL_STRING("Secure endpoint", route5.description.c_str());
}

// Test ApiRouteCore path normalization edge cases (lines 87, 92, 95)
void test_api_route_path_normalization_edges() {
  auto handler = [](WebRequestCore &req, WebResponseCore &res) {};

  // Path starting with /api/ (line 87)
  ApiRouteCore route1("/api/", WebModuleCore::WM_GET, handler);
  TEST_ASSERT_EQUAL_STRING("/", route1.webRoute.path.c_str());

  // Just "api" (line 92)
  ApiRouteCore route2("api", WebModuleCore::WM_POST, handler);
  TEST_ASSERT_EQUAL_STRING("/", route2.webRoute.path.c_str());

  // Path without leading slash (line 95)
  ApiRouteCore route3("test", WebModuleCore::WM_PUT, handler);
  TEST_ASSERT_EQUAL_STRING("/test", route3.webRoute.path.c_str());

  // Path with leading slash but no /api/
  ApiRouteCore route4("/users", WebModuleCore::WM_DELETE, handler);
  TEST_ASSERT_EQUAL_STRING("/users", route4.webRoute.path.c_str());

  // Complex /api/ path
  ApiRouteCore route5("/api/v2/users/123", WebModuleCore::WM_PATCH, handler);
  TEST_ASSERT_EQUAL_STRING("/v2/users/123", route5.webRoute.path.c_str());

  // Path that contains "api" but doesn't start with /api/
  ApiRouteCore route6("/erapid", WebModuleCore::WM_GET, handler);
  TEST_ASSERT_EQUAL_STRING("/erapid", route6.webRoute.path.c_str());
}

// Test various content types to hit all branches
void test_various_content_types() {
  auto handler = [](WebRequestCore &req, WebResponseCore &res) {};

  // Different content types
  std::vector<std::string> contentTypes = {"text/html",
                                           "text/plain",
                                           "application/json",
                                           "application/xml",
                                           "application/x-www-form-urlencoded",
                                           "multipart/form-data",
                                           "image/png",
                                           "audio/mpeg",
                                           ""};

  for (const auto &ct : contentTypes) {
    WebRouteCore route("/test", WebModuleCore::WM_GET, handler, ct);
    TEST_ASSERT_EQUAL_STRING(ct.c_str(), route.contentType.c_str());

    ApiRouteCore apiRoute("/api/test", WebModuleCore::WM_POST, handler, ct);
    TEST_ASSERT_EQUAL_STRING(ct.c_str(), apiRoute.webRoute.contentType.c_str());
  }
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
  RUN_TEST(test_web_route_core_empty_strings);
  RUN_TEST(test_api_route_core_empty_strings);
  RUN_TEST(test_all_http_methods);
  RUN_TEST(test_auth_requirements_variations);
  RUN_TEST(test_api_route_path_normalization_edges);
  RUN_TEST(test_various_content_types);
}
