#ifdef NATIVE_PLATFORM
// Native testing implementation of WebResponse

#include <interface/web_response.h>

WebResponse::WebResponse() : core(), jsonDoc(nullptr), isJsonContent(false) {}

void WebResponse::setStatus(int code) { core.setStatus(code); }

void WebResponse::setContent(const String &content, const String &mimeType) {
  core.setContent(content.c_str(), mimeType.c_str());
  isJsonContent = false;
}

void WebResponse::setProgmemContent(const char *progmemData,
                                    const String &mimeType) {
  core.setProgmemContent(progmemData, mimeType.c_str());
  isJsonContent = false;
}

void WebResponse::setHeader(const String &name, const String &value) {
  core.setHeader(name.c_str(), value.c_str());
}

void WebResponse::redirect(const String &url, int code) {
  core.setRedirect(url.c_str(), code);
}

String WebResponse::getContent() const {
  if (core.hasProgmemContent() && core.getProgmemData()) {
    return String(
        core.getProgmemData()); // In native testing, just convert to String
  }
  return String(core.getContent().c_str());
}

String WebResponse::getHeader(const String &name) const {
  std::string value = core.getHeader(name.c_str());
  return String(value.c_str());
}

// Stub implementations for methods that don't apply to native testing
void WebResponse::setJsonContent(const JsonDocument &doc) {
  jsonDoc = &doc;
  isJsonContent = true;
  core.setJsonContent("application/json");
  core.setStatus(200);
  core.setHeader("Content-Type", "application/json");
}

void WebResponse::setStorageStreamContent(const String &collection,
                                          const String &key,
                                          const String &mimeType,
                                          const String &driverName) {
  std::string driver =
      (driverName.length() == 0) ? "littlefs" : std::string(driverName.c_str());
  core.setStorageStreamContent(collection.c_str(), key.c_str(),
                               mimeType.c_str(), driver);
  isJsonContent = false;
}

void WebResponse::sendTo(WebServerClass *server) {
  // Stub for native testing - WebServerClass doesn't exist in native
  core.markResponseSent();
}

#endif // NATIVE_PLATFORM