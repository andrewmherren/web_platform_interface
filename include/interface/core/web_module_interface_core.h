#ifndef WEB_MODULE_INTERFACE_CORE_H
#define WEB_MODULE_INTERFACE_CORE_H

#include <functional>
#include <interface/core/auth_types_core.h>
#include <interface/core/web_request_core.h>
#include <interface/core/web_response_core.h>
#include <string>
#include <vector>

namespace WebModuleCore {

// HTTP Methods enum - pure C++ version
enum Method {
  WM_GET = 0,
  WM_POST = 1,
  WM_PUT = 2,
  WM_DELETE = 3,
  WM_PATCH = 4
};

// Core handler types using pure C++ types
typedef std::function<void(WebRequestCore &, WebResponseCore &)>
    UnifiedRouteHandler;

} // namespace WebModuleCore

#endif // WEB_MODULE_INTERFACE_CORE_H
