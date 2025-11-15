// Cross-platform String compatibility layer
#ifndef WEB_PLATFORM_INTERFACE_STRING_COMPAT_H
#define WEB_PLATFORM_INTERFACE_STRING_COMPAT_H

#if defined(ARDUINO) || defined(ESP_PLATFORM)
// Real Arduino environment
#include <Arduino.h>
#elif defined(NATIVE_PLATFORM)
// Native test environment with ArduinoFake
// ArduinoFake provides its own String class, so just include it
#include <Arduino.h>
#else
// Pure native build without Arduino at all - provide Arduino String
// compatibility
#include <algorithm>
#include <cstring>
#include <string>


class String : public std::string {
public:
  // Inherit constructors
  using std::string::string;
  String() : std::string() {}
  String(const char *cstr) : std::string(cstr ? cstr : "") {}
  String(const std::string &str) : std::string(str) {}

  // Arduino String compatibility methods
  bool startsWith(const String &prefix) const {
    return this->size() >= prefix.size() &&
           this->substr(0, prefix.size()) == prefix;
  }

  bool endsWith(const String &suffix) const {
    return this->size() >= suffix.size() &&
           this->substr(this->size() - suffix.size()) == suffix;
  }

  String substring(int beginIndex) const {
    if (beginIndex < 0)
      beginIndex = 0;
    if (beginIndex >= (int)this->size())
      return String();
    return String(this->substr(beginIndex));
  }

  String substring(int beginIndex, int endIndex) const {
    if (beginIndex < 0)
      beginIndex = 0;
    if (endIndex > (int)this->size())
      endIndex = this->size();
    if (beginIndex >= endIndex)
      return String();
    return String(this->substr(beginIndex, endIndex - beginIndex));
  }

  int indexOf(const String &str) const {
    auto pos = this->find(str);
    return pos == std::string::npos ? -1 : (int)pos;
  }

  int indexOf(char c) const {
    auto pos = this->find(c);
    return pos == std::string::npos ? -1 : (int)pos;
  }

  void toLowerCase() {
    std::transform(this->begin(), this->end(), this->begin(), ::tolower);
  }

  void toUpperCase() {
    std::transform(this->begin(), this->end(), this->begin(), ::toupper);
  }

  int toInt() const {
    try {
      return std::stoi(*this);
    } catch (...) {
      return 0;
    }
  }

  float toFloat() const {
    try {
      return std::stof(*this);
    } catch (...) {
      return 0.0f;
    }
  }
};
#endif

#endif // WEB_PLATFORM_INTERFACE_STRING_COMPAT_H
