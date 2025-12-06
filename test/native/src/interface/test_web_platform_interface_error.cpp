#include <stdexcept>
#include <unity.h>
#include <web_platform_interface.h>


// Test WebPlatformInterface::getPlatformInstance error path (line 134)
void test_web_platform_interface_uninitialized_error() {
  // Save current instance
  IWebPlatformProvider *savedInstance = IWebPlatformProvider::instance;

  // Clear the instance to force error condition
  IWebPlatformProvider::instance = nullptr;

#ifdef NATIVE_PLATFORM
  // In native builds, should throw std::runtime_error
  bool exceptionThrown = false;
  try {
    IWebPlatform &platform = IWebPlatformProvider::getPlatformInstance();
    (void)platform; // Suppress unused warning
  } catch (const std::runtime_error &e) {
    exceptionThrown = true;
    TEST_ASSERT_EQUAL_STRING("WebPlatform provider not initialized", e.what());
  }
  TEST_ASSERT_TRUE(exceptionThrown);
#else
  // In Arduino builds, enters infinite loop (can't test easily)
  TEST_PASS();
#endif

  // Restore instance
  IWebPlatformProvider::instance = savedInstance;
}

// Registration function
void register_web_platform_interface_error_tests() {
  RUN_TEST(test_web_platform_interface_uninitialized_error);
}
