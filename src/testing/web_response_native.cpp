#ifdef NATIVE_PLATFORM
// Native testing implementation of WebResponse

#include <interface/web_response.h>

WebResponse::WebResponse() : statusCode(200), headersSent(false), responseSent(false), 
                             progmemData(nullptr), isProgmemContent(false), 
                             jsonDoc(nullptr), isJsonContent(false), 
                             isStorageStreamContent(false) {
}

void WebResponse::setStatus(int code) {
    statusCode = code;
}

void WebResponse::setContent(const String &content, const String &mimeType) {
    this->content = content;
    this->mimeType = mimeType;
    isProgmemContent = false;
    isJsonContent = false;
    isStorageStreamContent = false;
}

void WebResponse::setProgmemContent(const char *progmemData, const String &mimeType) {
    this->progmemData = progmemData;
    this->mimeType = mimeType;
    isProgmemContent = true;
    isJsonContent = false;
    isStorageStreamContent = false;
}

void WebResponse::setHeader(const String &name, const String &value) {
    headers[name] = value;
}

void WebResponse::redirect(const String &url, int code) {
    setStatus(code);
    setHeader("Location", url);
}

String WebResponse::getContent() const {
    if (isProgmemContent && progmemData) {
        return String(progmemData);  // In native testing, just convert to String
    }
    return content;
}

String WebResponse::getHeader(const String &name) const {
    auto it = headers.find(name);
    return (it != headers.end()) ? it->second : String("");
}

// Stub implementations for methods that don't apply to native testing
void WebResponse::setJsonContent(const JsonDocument &doc) {
    // For native testing, we could serialize to string if needed
}

void WebResponse::setStorageStreamContent(const String &collection, const String &key,
                                         const String &mimeType, const String &driverName) {
    // Stub for native testing
}

void WebResponse::sendTo(WebServerClass *server) {
    // Stub for native testing - WebServerClass doesn't exist in native
    responseSent = true;
}

#endif // NATIVE_PLATFORM