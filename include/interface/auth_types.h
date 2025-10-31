#ifndef AUTH_TYPES_H
#define AUTH_TYPES_H

#include <Arduino.h>
#include <interface/core/auth_types_core.h>

/**
 * Authentication Types for Route Protection
 *
 * This enum defines the types of authentication that can be required
 * for accessing routes. Multiple auth types can be combined to allow
 * alternative authentication methods (OR logic).
 */
// AuthType and AuthRequirements are provided by the core header above.

/**
 * Authentication Context
 *
 * Contains information about the current authentication state
 * for a request, populated during authentication middleware processing.
 */
struct AuthContext {
  bool isAuthenticated = false;
  AuthType authenticatedVia = AuthType::NONE;
  String sessionId;
  String token;
  String username;
  unsigned long authenticatedAt = 0; // Timestamp of authentication

  AuthContext() = default;

  // Helper methods
  bool hasValidSession() const {
    return isAuthenticated && authenticatedVia == AuthType::SESSION &&
           sessionId.length() > 0;
  }

  bool hasValidToken() const {
    return isAuthenticated && authenticatedVia == AuthType::TOKEN &&
           token.length() > 0;
  }

  void clear() {
    isAuthenticated = false;
    authenticatedVia = AuthType::NONE;
    sessionId = "";
    token = "";
    username = "";
    authenticatedAt = 0;
  }
};

/**
 * Helper functions for working with AuthType
 */
namespace AuthUtils {
// Check if an AuthType is present in requirements
bool hasAuthType(const AuthRequirements &requirements, AuthType type);

// Check if any authentication is required
bool requiresAuth(const AuthRequirements &requirements);

// Convert AuthType to string for debugging
String authTypeToString(AuthType type);

} // namespace AuthUtils

#endif // AUTH_TYPES_H