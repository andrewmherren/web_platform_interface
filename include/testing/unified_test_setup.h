#ifndef UNIFIED_TEST_SETUP_H
#define UNIFIED_TEST_SETUP_H

#include "testing_platform_provider.h"
#include <memory>

/**
 * Unified test setup helper for all modules.
 * This provides a consistent way to set up testing across all modules.
 */
class UnifiedTestSetup {
public:
    // Create a mock platform provider for dependency injection
    static std::unique_ptr<MockWebPlatformProvider> createMockProvider() {
        return std::make_unique<MockWebPlatformProvider>();
    }
    
    // Set the global platform provider for tests
    static void setGlobalProvider(MockWebPlatformProvider* provider) {
        IWebPlatformProvider::instance = provider;
    }
    
    // Clear the global platform provider
    static void clearGlobalProvider() {
        IWebPlatformProvider::instance = nullptr;
    }
    
    // Standard setup function for tests
    static void setUp() {
        auto provider = createMockProvider();
        setGlobalProvider(provider.get());
        return provider;
    }
    
    // Standard teardown function for tests
    static void tearDown() {
        clearGlobalProvider();
    }
};

#endif // UNIFIED_TEST_SETUP_H