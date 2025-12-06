#ifndef AUTH_TYPES_CORE_H
#define AUTH_TYPES_CORE_H

#include <vector>

/**
 * Core Authentication Types (pure C++)
 *
 * These definitions are Arduino-agnostic and safe for native builds.
 */
enum class AuthType {
  NONE = 0,       // No authentication required
  SESSION = 1,    // Session-based authentication (cookie)
  TOKEN = 2,      // Token-based authentication (header/param)
  LOCAL_ONLY = 4, // Local network access only
  PAGE_TOKEN = 8  // CSRF protection for pages
};

// Container of allowed authentication methods (OR logic)
using AuthRequirements = std::vector<AuthType>;

#endif // AUTH_TYPES_CORE_H
