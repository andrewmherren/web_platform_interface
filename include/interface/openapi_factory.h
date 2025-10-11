#ifndef OPENAPI_FACTORY_H
#define OPENAPI_FACTORY_H

#include <interface/openapi_types.h>

/**
 * Factory class for creating common OpenAPI documentation patterns
 * This is a helper class to make creating OpenAPI documentation easier
 */
class OpenAPIFactory {
public:
  /**
   * Create a basic OpenAPI documentation object
   */
  static OpenAPIDocumentation create(const String &summary,
                                     const String &description = "",
                                     const String &operationId = "",
                                     const std::vector<String> &tags = {}) {
    return OpenAPIDocumentation(summary, description, operationId, tags);
  }

  /**
   * Create a standard success response schema
   */
  static String createSuccessResponse(
      const String &description = "Operation completed successfully") {
    return R"({
            "type": "object",
            "properties": {
                "success": {
                    "type": "boolean",
                    "description": "Whether the operation was successful",
                    "example": true
                },
                "message": {
                    "type": "string",
                    "description": ")" +
           description + R"(",
                    "example": "Operation successful"
                }
            },
            "required": ["success"]
        })";
  }

  /**
   * Create a standard error response schema
   */
  static String
  createErrorResponse(const String &description = "Details about the error") {
    return R"({
            "type": "object",
            "properties": {
                "success": {
                    "type": "boolean",
                    "description": "Whether the operation was successful",
                    "example": false
                },
                "error": {
                    "type": "string",
                    "description": ")" +
           description + R"(",
                    "example": "Operation failed"
                }
            },
            "required": ["success", "error"]
        })";
  }

  /**
   * Create a list/array response schema
   */
  static String createListResponse(const String &itemDescription) {
    return R"({
            "type": "object",
            "properties": {
                "items": {
                    "type": "array",
                    "items": {},
                    "description": "List of )" +
           itemDescription + R"("
                },
                "total": {
                    "type": "integer",
                    "description": "Total number of items"
                }
            },
            "required": ["items", "total"]
        })";
  }

  /**
   * Create an ID parameter schema for path parameters
   */
  static String createIdParameter(const String &name,
                                  const String &description) {
    return R"({
            "name": ")" +
           name + R"(",
            "in": "path",
            "required": true,
            "schema": {"type": "string"},
            "description": ")" +
           description + R"("
        })";
  }

  /**
   * Generate operation ID from method and resource
   */
  static String generateOperationId(const String &method,
                                    const String &resource) {
    String firstChar = String(resource.charAt(0));
    String restOfString = resource.substring(1);

    // Convert to lowercase (platform-compatible way)
    for (unsigned int i = 0; i < restOfString.length(); i++) {
      if (restOfString[i] >= 'A' && restOfString[i] <= 'Z') {
        restOfString[i] = restOfString[i] + 32; // Convert to lowercase
      }
    }

    return method + firstChar + restOfString;
  }

  /**
   * Format tag name for OpenAPI documentation
   */
  static String formatTag(const String &moduleName) { return moduleName; }

  /**
   * Create OpenAPI documentation with success response
   */
  static OpenAPIDocumentation
  createWithSuccessResponse(const String &summary, const String &description,
                            const String &operationId,
                            const std::vector<String> &tags,
                            const String &responseDescription) {
    return OpenAPIDocumentation(summary, description, operationId, tags)
        .withResponseSchema(createSuccessResponse(responseDescription));
  }
};

#endif // OPENAPI_FACTORY_H