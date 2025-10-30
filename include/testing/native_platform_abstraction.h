#ifndef NATIVE_PLATFORM_ABSTRACTION_H
#define NATIVE_PLATFORM_ABSTRACTION_H

#ifdef NATIVE_PLATFORM

// Native platform abstraction for module testing
// This provides ESP32-compatible APIs for native test environments
// so module developers can write tests that work on both platforms

#include <ArduinoFake.h>
#include <interface/string_compat.h>
#include <testing/arduino_string_compat.h>
#include <string>
#include <cstdlib>
#include <ctime>
#include <random>

// ESP32 WiFi authentication constants for native testing
#ifndef WIFI_AUTH_OPEN
#define WIFI_AUTH_OPEN 0
#define WIFI_AUTH_WEP 1
#define WIFI_AUTH_WPA_PSK 2
#define WIFI_AUTH_WPA2_PSK 3
#define WIFI_AUTH_WPA_WPA2_PSK 4
#define WIFI_AUTH_WPA2_ENTERPRISE 5
#define WIFI_AUTH_WPA3_PSK 6
#define WIFI_AUTH_WPA2_WPA3_PSK 7
#endif

// ESP32 random number generation abstraction
namespace NativePlatform {
    static bool randomInitialized = false;
    
    inline void initRandom() {
        if (!randomInitialized) {
            std::srand(std::time(nullptr));
            randomInitialized = true;
        }
    }
    
    inline uint32_t mockEspRandom() {
        initRandom();
        return std::rand();
    }
    
    inline void mockEspFillRandom(void* buf, size_t len) {
        initRandom();
        uint8_t* bytes = static_cast<uint8_t*>(buf);
        for (size_t i = 0; i < len; i++) {
            bytes[i] = std::rand() % 256;
        }
    }
}

// Mock ESP32 hardware functions for native testing
#ifndef esp_random
#define esp_random() NativePlatform::mockEspRandom()
#endif
#ifndef esp_fill_random
#define esp_fill_random(buf, len) NativePlatform::mockEspFillRandom(buf, len)
#endif

// Mock mbedTLS structures and functions for crypto testing
#ifndef MBEDTLS_MD_SHA256
#define MBEDTLS_MD_SHA256 1
#endif

struct mbedtls_md_context_t { int dummy; };
struct mbedtls_md_info_t { int dummy; };

// Mock mbedTLS functions with realistic behavior for testing
namespace NativeCrypto {
    inline void mockMdInit(mbedtls_md_context_t* ctx) { (void)ctx; }
    inline void mockMdFree(mbedtls_md_context_t* ctx) { (void)ctx; }
    
    inline int mockMdSetup(mbedtls_md_context_t* ctx, const mbedtls_md_info_t* info, int hmac) { 
        (void)ctx; (void)info; (void)hmac; return 0; 
    }
    
    inline const mbedtls_md_info_t* mockMdInfoFromType(int type) { 
        (void)type; return reinterpret_cast<const mbedtls_md_info_t*>(1); 
    }

    // Mock PBKDF2 with deterministic output for testing
    inline int mockPbkdf2Hmac(
        mbedtls_md_context_t* ctx, 
        const unsigned char* password, size_t plen,
        const unsigned char* salt, size_t slen, 
        unsigned int iteration_count,
        uint32_t key_length, unsigned char* output) {
        
        (void)ctx;
        // Create deterministic but scrambled output for testing
        for(size_t i = 0; i < key_length; i++) {
            uint8_t p = (i < plen) ? password[i] : password[i % plen];
            uint8_t s = (i < slen) ? salt[i] : salt[i % slen];
            output[i] = (p ^ s ^ ((i + iteration_count) & 0xFF)) & 0xFF;
        }
        return 0;
    }
}

// Define mbedTLS function macros for native testing
#ifndef mbedtls_md_init
#define mbedtls_md_init(ctx) NativeCrypto::mockMdInit(ctx)
#endif
#ifndef mbedtls_md_free
#define mbedtls_md_free(ctx) NativeCrypto::mockMdFree(ctx)
#endif
#ifndef mbedtls_md_setup
#define mbedtls_md_setup(ctx, info, hmac) NativeCrypto::mockMdSetup(ctx, info, hmac)
#endif
#ifndef mbedtls_md_info_from_type
#define mbedtls_md_info_from_type(type) NativeCrypto::mockMdInfoFromType(type)
#endif
#ifndef mbedtls_pkcs5_pbkdf2_hmac
#define mbedtls_pkcs5_pbkdf2_hmac(ctx, pwd, plen, salt, slen, iter, klen, out) \
    NativeCrypto::mockPbkdf2Hmac(ctx, pwd, plen, salt, slen, iter, klen, out)
#endif

#endif // NATIVE_PLATFORM

#endif // NATIVE_PLATFORM_ABSTRACTION_H