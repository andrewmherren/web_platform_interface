#include <algorithm>
#include <interface/core/web_response_core.h>


WebResponseCore::WebResponseCore()
    : statusCode_(200), content_(""), mimeType_("text/html"),
      headersSent_(false), responseSent_(false),
      contentType_(ContentType::Normal), progmemData_(nullptr),
      storageCollection_(""), storageKey_(""), storageDriverName_("") {}

void WebResponseCore::setStatus(int code) { statusCode_ = code; }

void WebResponseCore::setContent(const std::string &content,
                                 const std::string &mimeType) {
  content_ = content;
  mimeType_ = mimeType;
  contentType_ = ContentType::Normal;
  progmemData_ = nullptr;
  storageCollection_.clear();
  storageKey_.clear();
}

void WebResponseCore::setProgmemContent(const char *progmemData,
                                        const std::string &mimeType) {
  progmemData_ = progmemData;
  mimeType_ = mimeType;
  contentType_ = ContentType::Progmem;
  content_.clear();
  storageCollection_.clear();
  storageKey_.clear();
}

void WebResponseCore::setJsonContent(const std::string &mimeType) {
  mimeType_ = mimeType;
  contentType_ = ContentType::Json;
  content_.clear();
  progmemData_ = nullptr;
  storageCollection_.clear();
  storageKey_.clear();
}

void WebResponseCore::setStorageStreamContent(const std::string &collection,
                                              const std::string &key,
                                              const std::string &mimeType,
                                              const std::string &driverName) {
  storageCollection_ = collection;
  storageKey_ = key;
  storageDriverName_ = driverName;
  mimeType_ = mimeType;
  contentType_ = ContentType::Storage;
  content_.clear();
  progmemData_ = nullptr;
}

void WebResponseCore::setMimeType(const std::string &mimeType) {
  mimeType_ = mimeType;
}

void WebResponseCore::setHeader(const std::string &name,
                                const std::string &value) {
  headers_[name] = value;
}

std::string WebResponseCore::getHeader(const std::string &name) const {
  auto it = headers_.find(name);
  if (it != headers_.end()) {
    return it->second;
  }
  return "";
}

bool WebResponseCore::hasHeader(const std::string &name) const {
  return headers_.find(name) != headers_.end();
}

void WebResponseCore::setRedirect(const std::string &url, int code) {
  statusCode_ = code;
  setHeader("Location", url);
  content_.clear();
  contentType_ = ContentType::Normal;
}

void WebResponseCore::reset() {
  statusCode_ = 200;
  content_.clear();
  mimeType_ = "text/html";
  headers_.clear();
  headersSent_ = false;
  responseSent_ = false;
  contentType_ = ContentType::Normal;
  progmemData_ = nullptr;
  storageCollection_.clear();
  storageKey_.clear();
  storageDriverName_.clear();
}
