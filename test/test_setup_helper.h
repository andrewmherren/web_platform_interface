#include <unity.h>
#include "testing/testing_platform_provider.h"
#include "maker_api.h"

std::unique_ptr<MockWebPlatformProvider> mockProvider;
MakerAPIModule* testModule;

void setUp() {
    mockProvider = std::make_unique<MockWebPlatformProvider>();
    IWebPlatformProvider::instance = mockProvider.get();

    testModule = new MakerAPIModule();
    testModule->begin();
}

void tearDown() {
    delete testModule;
    mockProvider.reset();
    IWebPlatformProvider::instance = nullptr;
}

void test_something() {
    testModule->handle();
    TEST_ASSERT_TRUE(mockProvider->getMockPlatform().isConnected());
}
