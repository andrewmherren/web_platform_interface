#ifndef WEB_PLATFORM_INTERFACE_WEBSERVER_TYPEDEFS_H
#define WEB_PLATFORM_INTERFACE_WEBSERVER_TYPEDEFS_H

#ifndef NATIVE_PLATFORM
#include <Arduino.h>
#include <WebServer.h>
// Define WebServerClass as a concrete class derived from WebServer
class WebServerClass : public WebServer {
public:
  WebServerClass(uint16_t port = 80) : WebServer(port) {}
};
#else
// For native/testing environments
#include <cstdint>
class WebServerClass {
public:
  WebServerClass(uint16_t port = 80) {}
  // Add mock methods as needed for testing
};
#endif

#endif // WEB_PLATFORM_INTERFACE_WEBSERVER_TYPEDEFS_H