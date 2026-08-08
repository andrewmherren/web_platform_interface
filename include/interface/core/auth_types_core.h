#ifndef AUTH_TYPES_CORE_H
#define AUTH_TYPES_CORE_H

#include <vector>

/**
 * Core Authentication Types (pure C++)
 *
 * These definitions are Arduino-agnostic and safe for native builds.
 *
 * A plain enum, not a bitmask: AuthRequirements is a std::vector<AuthType>
 * checked by linear scan (OR logic - any listed type satisfies the
 * requirement), so values don't need to be independent bits.
 */
enum class AuthType {
  NONE,       // No authentication required
  SESSION,    // Session-based authentication (cookie)
  TOKEN,      // Token-based authentication (header/param)
  LOCAL_ONLY, // Local network access only
  PAGE_TOKEN  // CSRF protection for pages
};

// Container of allowed authentication methods (OR logic)
using AuthRequirements = std::vector<AuthType>;

#endif // AUTH_TYPES_CORE_H
