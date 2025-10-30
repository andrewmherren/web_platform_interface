#ifdef NATIVE_PLATFORM

#include <testing/native_platform_abstraction.h>

// Define global instances for ESP32-specific mock classes
// Note: WiFi and EEPROM are provided by ArduinoFake
DNSServer dnsServer;
ESPmDNS MDNS;
ESPClass ESP;

#endif // NATIVE_PLATFORM