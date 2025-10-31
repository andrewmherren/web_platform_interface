#ifndef WEB_RESPONSE_CORE_H
#define WEB_RESPONSE_CORE_H

#include <cstddef>
#include <map>
#include <string>


/**
 * WebResponseCore - Pure C++ core logic for HTTP response handling
 *
 * Platform-agnostic response data and logic that can be tested
 * in native C++ environments without Arduino dependencies.
 *
 * The Arduino WebResponse class wraps this core and provides
 * Arduino String-based APIs for backward compatibility.
 */
class WebResponseCore {
public:
  enum class ContentType {
    Normal,  // Regular string content
    Progmem, // PROGMEM data (pointer only, no ownership)
    Json,    // JSON document (external ownership)
    Storage  // Storage stream (collection/key reference)
  };

private:
  int statusCode_;
  std::string content_;
  std::string mimeType_;
  std::map<std::string, std::string> headers_;
  bool headersSent_;
  bool responseSent_;

  // Content type tracking
  ContentType contentType_;

  // PROGMEM content (non-owning pointer)
  const char *progmemData_;

  // Storage stream parameters
  std::string storageCollection_;
  std::string storageKey_;
  std::string storageDriverName_;

public:
  WebResponseCore();

  // Response configuration
  void setStatus(int code);
  int getStatus() const { return statusCode_; }

  void setContent(const std::string &content,
                  const std::string &mimeType = "text/html");
  std::string getContent() const { return content_; }

  void setProgmemContent(const char *progmemData, const std::string &mimeType);
  bool hasProgmemContent() const {
    return contentType_ == ContentType::Progmem;
  }
  const char *getProgmemData() const { return progmemData_; }

  void setJsonContent(const std::string &mimeType = "application/json");
  bool hasJsonContent() const { return contentType_ == ContentType::Json; }

  void setStorageStreamContent(const std::string &collection,
                               const std::string &key,
                               const std::string &mimeType,
                               const std::string &driverName = "");
  bool hasStorageStreamContent() const {
    return contentType_ == ContentType::Storage;
  }
  std::string getStorageCollection() const { return storageCollection_; }
  std::string getStorageKey() const { return storageKey_; }
  std::string getStorageDriverName() const { return storageDriverName_; }

  void setMimeType(const std::string &mimeType);
  std::string getMimeType() const { return mimeType_; }

  void setHeader(const std::string &name, const std::string &value);
  std::string getHeader(const std::string &name) const;
  bool hasHeader(const std::string &name) const;
  const std::map<std::string, std::string> &getHeaders() const {
    return headers_;
  }

  void setRedirect(const std::string &url, int code = 302);

  // Status tracking
  void markHeadersSent() { headersSent_ = true; }
  void markResponseSent() { responseSent_ = true; }
  bool isHeadersSent() const { return headersSent_; }
  bool isResponseSent() const { return responseSent_; }

  ContentType getContentType() const { return contentType_; }

  // Reset state
  void reset();
};

#endif // WEB_RESPONSE_CORE_H
