#ifndef WEB_RESPONSE_H
#define WEB_RESPONSE_H

#include <Arduino.h>
#include <ArduinoJson.h>
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
 */
class WebResponse {
private:
  int statusCode;
  String content;
  String mimeType;
  std::map<String, String> headers;
  bool headersSent;
  bool responseSent;
  const char *progmemData;
  bool isProgmemContent;
  const JsonDocument *jsonDoc;
  bool isJsonContent;
  String storageCollection;
  String storageKey;
  String storageDriverName;
  bool isStorageStreamContent;

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

  bool hasProgmemContent() const { return isProgmemContent; }
  const char *getProgmemData() const { return progmemData; }

  // Send response (called internally by WebPlatform)
  void sendTo(WebServerClass *server);

  esp_err_t sendTo(httpd_req *req);

  // Status queries
  // bool isHeadersSent() const { return headersSent; }
  bool isResponseSent() const { return responseSent; }
  // int getStatus() const { return statusCode; }
  String getContent() const;
  String getMimeType() const { return mimeType; }

  // Header access
  String getHeader(const String &name) const;

private:
  void markHeadersSent() { headersSent = true; }
  void markResponseSent() { responseSent = true; }

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