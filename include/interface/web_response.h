#ifndef WEB_RESPONSE_H
#define WEB_RESPONSE_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <interface/core/web_response_core.h>
#include <interface/webserver_typedefs.h>
#include <map>


struct httpd_req;
typedef int esp_err_t;

/**
 * WebResponse - Unified response abstraction for HTTP/HTTPS handlers
 *
 * Part of the web_module_interface to provide a consistent interface
 * for sending responses across Arduino WebServer and ESP-IDF HTTP
 * server implementations without modules needing to know about
 * WebPlatform internals.
 *
 * This class wraps WebResponseCore (pure C++) and provides Arduino
 * String-based APIs for backward compatibility. The core handles
 * platform-agnostic logic that can be tested in native environments.
 */
class WebResponse {
private:
  WebResponseCore core; // Pure C++ core for platform-agnostic logic

  // Arduino-specific state (not in core)
  const JsonDocument *jsonDoc; // External JSON document (non-owning)
  bool isJsonContent;

public:
  WebResponse();

  // Response configuration
  void setStatus(int code);
  void setContent(const String &content, const String &mimeType = "text/html");
  void setProgmemContent(const char *progmemData, const String &mimeType);
  void setJsonContent(const JsonDocument &doc);
  void setStorageStreamContent(const String &collection, const String &key,
                               const String &mimeType,
                               const String &driverName = "");
  void setHeader(const String &name, const String &value);
  void redirect(const String &url, int code = 302);

  bool hasProgmemContent() const { return core.hasProgmemContent(); }
  const char *getProgmemData() const { return core.getProgmemData(); }

  // Send response (called internally by WebPlatform)
  void sendTo(WebServerClass *server);

  esp_err_t sendTo(httpd_req *req);

  // Status queries
  bool isResponseSent() const { return core.isResponseSent(); }
  int getStatus() const { return core.getStatus(); }
  String getContent() const;
  String getMimeType() const { return String(core.getMimeType().c_str()); }

  // Header access
  String getHeader(const String &name) const;

  // Core access for advanced usage and testing
  const WebResponseCore &getCore() const { return core; }
  WebResponseCore &getCore() { return core; }

private:
  void markHeadersSent() { core.markHeadersSent(); }
  void markResponseSent() { core.markResponseSent(); }

  // Helper methods for PROGMEM streaming
  void sendProgmemChunked(const char *data, WebServerClass *server);
  esp_err_t sendProgmemChunked(const char *data, httpd_req *req);

  // JSON streaming helper
  void streamJsonContent(const JsonDocument &doc, WebServerClass *server);
  esp_err_t streamJsonContent(const JsonDocument &doc, httpd_req *req);

  // Storage-based streaming helpers
  void streamFromStorage(const String &collection, const String &key,
                         WebServerClass *server, const String &driverName = "");
  esp_err_t streamFromStorage(const String &collection, const String &key,
                              httpd_req *req, const String &driverName = "");

  // Allow WebPlatform to call private methods
  friend class WebPlatform;
};

#endif // WEB_RESPONSE_H