#ifdef NATIVE_PLATFORM
// Native testing implementation of RouteVariant

#include <cstring>
#include <interface/core/web_request_core.h>
#include <interface/core/web_response_core.h>
#include <interface/utils/route_variant.h>
#include <interface/web_module_interface.h>

RouteVariant::RouteVariant(const WebRoute &route) : type(WEB_ROUTE) {
  webRoute = new WebRoute(route);
}

RouteVariant::RouteVariant(const ApiRoute &route) : type(API_ROUTE) {
  apiRoute = new ApiRoute(route);
}

RouteVariant::RouteVariant(const RouteVariant &other) : type(other.type) {
  if (type == WEB_ROUTE) {
    webRoute = new WebRoute(*other.webRoute);
  } else {
    apiRoute = new ApiRoute(*other.apiRoute);
  }
}

RouteVariant &RouteVariant::operator=(const RouteVariant &other) {
  if (this != &other) {
    // Clean up existing
    if (type == WEB_ROUTE) {
      delete webRoute;
    } else {
      delete apiRoute;
    }

    // Copy from other
    type = other.type;
    if (type == WEB_ROUTE) {
      webRoute = new WebRoute(*other.webRoute);
    } else {
      apiRoute = new ApiRoute(*other.apiRoute);
    }
  }
  return *this;
}

RouteVariant::~RouteVariant() {
  if (type == WEB_ROUTE) {
    delete webRoute;
  } else {
    delete apiRoute;
  }
}

const WebRoute &RouteVariant::getWebRoute() const {
  if (type != WEB_ROUTE) {
    // For native testing, we need a way to handle this error
    // In a real implementation this would throw an exception
    static auto dummyHandler = [](WebRequestCore &, WebResponseCore &) {};
    static WebRoute dummy("", WebModule::WM_GET, dummyHandler,
                          AuthRequirements{});
    return dummy;
  }
  return *webRoute;
}

const ApiRoute &RouteVariant::getApiRoute() const {
  if (type != API_ROUTE) {
    // For native testing, we need a way to handle this error
    static auto dummyHandler = [](WebRequestCore &, WebResponseCore &) {};
    static ApiRoute dummy("", WebModule::WM_GET, dummyHandler,
                          AuthRequirements{}, OpenAPIDocumentation());
    return dummy;
  }
  return *apiRoute;
}

// Template specializations for helper functions
template <> bool holds_alternative<WebRoute>(const RouteVariant &v) {
  return v.isWebRoute();
}

template <> bool holds_alternative<ApiRoute>(const RouteVariant &v) {
  return v.isApiRoute();
}

template <> const WebRoute &get<WebRoute>(const RouteVariant &v) {
  return v.getWebRoute();
}

template <> const ApiRoute &get<ApiRoute>(const RouteVariant &v) {
  return v.getApiRoute();
}

#endif // NATIVE_PLATFORM