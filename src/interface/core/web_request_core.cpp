#include <algorithm>
#include <cctype>
#include <interface/core/web_request_core.h>
#include <sstream>

// For JSON parsing - we'll need a simple JSON parser or ArduinoJson
// compatibility
#ifdef ARDUINO
#include <ArduinoJson.h>
#else
// For native testing, we'll implement a simple JSON parser or use a compatible
// approach For now, we'll stub it and implement basic functionality
#endif

WebRequestCore::WebRequestCore() : path("/"), method(WebModule::WM_GET) {}

std::string
WebRequestCore::getRouteParameter(const std::string &paramName) const {
  // For core tests, route parameters are stored in the same params map
  auto it = params.find(paramName);
  if (it != params.end()) {
    return it->second;
  }
  return "";
}

std::string WebRequestCore::getParam(const std::string &name) const {
  auto it = params.find(name);
  return (it != params.end()) ? it->second : "";
}

std::string WebRequestCore::getHeader(const std::string &name) const {
  // HTTP headers are case-insensitive, so we need to search case-insensitively
  for (const auto &header : headers) {
    if (caseInsensitiveCompare(header.first, name) == 0) {
      return header.second;
    }
  }
  return "";
}

std::string WebRequestCore::getJsonParam(const std::string &name) const {
  auto it = jsonParams.find(name);
  return (it != jsonParams.end()) ? it->second : "";
}

void WebRequestCore::parseQueryParams(const std::string &query) {
  if (query.empty())
    return;

  std::stringstream ss(query);
  std::string pair;

  while (std::getline(ss, pair, '&')) {
    size_t pos = pair.find('=');
    if (pos != std::string::npos) {
      std::string key = urlDecode(pair.substr(0, pos));
      std::string value = urlDecode(pair.substr(pos + 1));
      params[key] = value;
    }
  }
}

void WebRequestCore::parseFormData(const std::string &formData) {
  // Form data uses the same format as query parameters
  parseQueryParams(formData);
}

void WebRequestCore::parseJsonData(const std::string &jsonData) {
  if (jsonData.empty())
    return;

#ifdef ARDUINO
  // Use ArduinoJson for Arduino builds
  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, jsonData.c_str());

  if (error) {
    return; // Invalid JSON
  }

  // Extract key-value pairs from JSON object
  if (doc.is<JsonObject>()) {
    JsonObject obj = doc.as<JsonObject>();
    for (JsonPair kv : obj) {
      const char *key = kv.key().c_str();
      if (kv.value().is<const char *>()) {
        jsonParams[key] = kv.value().as<const char *>();
      } else if (kv.value().is<int>()) {
        jsonParams[key] = std::to_string(kv.value().as<int>());
      } else if (kv.value().is<double>()) {
        jsonParams[key] = std::to_string(kv.value().as<double>());
      } else if (kv.value().is<bool>()) {
        jsonParams[key] = kv.value().as<bool>() ? "true" : "false";
      }
    }
  }
#else
  // For native testing, implement basic JSON parsing
  // This is a very simplified JSON parser for testing purposes only
  // Production code should use a proper JSON library

  // Skip whitespace and opening brace
  size_t pos = 0;
  while (pos < jsonData.length() && std::isspace(jsonData[pos]))
    pos++;
  if (pos >= jsonData.length() || jsonData[pos] != '{')
    return;
  pos++;

  while (pos < jsonData.length()) {
    // Skip whitespace
    while (pos < jsonData.length() && std::isspace(jsonData[pos]))
      pos++;
    if (pos >= jsonData.length() || jsonData[pos] == '}')
      break;

    // Parse key
    if (jsonData[pos] != '"')
      break;
    pos++;
    size_t keyStart = pos;
    while (pos < jsonData.length() && jsonData[pos] != '"')
      pos++;
    if (pos >= jsonData.length())
      break;
    std::string key = jsonData.substr(keyStart, pos - keyStart);
    pos++;

    // Skip to colon
    while (pos < jsonData.length() && std::isspace(jsonData[pos]))
      pos++;
    if (pos >= jsonData.length() || jsonData[pos] != ':')
      break;
    pos++;

    // Skip whitespace
    while (pos < jsonData.length() && std::isspace(jsonData[pos]))
      pos++;
    if (pos >= jsonData.length())
      break;

    // Parse value
    std::string value;
    if (jsonData[pos] == '"') {
      // String value
      pos++;
      size_t valueStart = pos;
      while (pos < jsonData.length() && jsonData[pos] != '"')
        pos++;
      if (pos >= jsonData.length())
        break;
      value = jsonData.substr(valueStart, pos - valueStart);
      pos++;
    } else {
      // Number or boolean
      size_t valueStart = pos;
      while (pos < jsonData.length() && jsonData[pos] != ',' &&
             jsonData[pos] != '}' && !std::isspace(jsonData[pos]))
        pos++;
      value = jsonData.substr(valueStart, pos - valueStart);
    }

    jsonParams[key] = value;

    // Skip to next pair or end
    while (pos < jsonData.length() && std::isspace(jsonData[pos]))
      pos++;
    if (pos < jsonData.length() && jsonData[pos] == ',')
      pos++;
  }
#endif
}

void WebRequestCore::parseRequestBody(const std::string &body,
                                      const std::string &contentType) {
  if (body.empty())
    return;

  // Convert content type to lowercase for comparison
  std::string lowerContentType = contentType;
  std::transform(lowerContentType.begin(), lowerContentType.end(),
                 lowerContentType.begin(), ::tolower);

  if (lowerContentType.find("application/json") != std::string::npos) {
    parseJsonData(body);
  } else if (lowerContentType.find("application/x-www-form-urlencoded") !=
             std::string::npos) {
    parseFormData(body);
  }
  // multipart/form-data would need more complex parsing - skip for now
}

std::string WebRequestCore::urlDecode(const std::string &str) {
  std::string result;
  result.reserve(str.length());

  for (size_t i = 0; i < str.length(); ++i) {
    if (str[i] == '+') {
      result += ' ';
    } else if (str[i] == '%' && i + 2 < str.length()) {
      // Parse hex digits
      char hex[3] = {str[i + 1], str[i + 2], 0};
      char *endptr;
      long value = strtol(hex, &endptr, 16);
      if (endptr == hex + 2) {
        result += static_cast<char>(value);
        i += 2;
      } else {
        result += str[i];
      }
    } else {
      result += str[i];
    }
  }

  return result;
}

// Case-insensitive string comparison for header lookups
int WebRequestCore::caseInsensitiveCompare(const std::string &s1,
                                           const std::string &s2) const {
  const char *str1 = s1.c_str();
  const char *str2 = s2.c_str();
  while (*str1 && *str2) {
    int c1 = std::tolower(*str1++);
    int c2 = std::tolower(*str2++);
    if (c1 != c2)
      return c1 - c2;
  }
  return std::tolower(*str1) - std::tolower(*str2);
}
