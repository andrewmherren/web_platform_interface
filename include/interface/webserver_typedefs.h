#ifndef WEB_PLATFORM_INTERFACE_WEBSERVER_TYPEDEFS_H
#define WEB_PLATFORM_INTERFACE_WEBSERVER_TYPEDEFS_H

#include <Arduino.h>
#ifndef NATIVE_PLATFORM
#include <WebServer.h>
// Define WebServerClass as a concrete class derived from WebServer
class WebServerClass : public WebServer {
public:
  WebServerClass(uint16_t port = 80) : WebServer(port) {}
};
#else
// For native/testing environments
class WebServerClass {
public:
  WebServerClass(uint16_t port = 80) {}
  // Add mock methods as needed for testing
};
#endif

#endif // WEB_PLATFORM_INTERFACE_WEBSERVER_TYPEDEFS_H