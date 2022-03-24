#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecureBearSSL.h>

// OpenWeatherMap
#define APPID "74be05f6b9c8eea10478aa9a2bb0ecb3"
#define CITY "Moscow"
#define UNITS "metric"
#define LANG "en"
#define URL "https://api.openweathermap.org/data/2.5/weather?q=" CITY "&appid=" APPID "&units=" UNITS "&lang=" LANG

// WiFi
#define ERROR_STR "Some error in funtction \"send_request(const char*)\"...."
#define MAX_BAD_REQUESTS 12
#define WIFI_SSID "SSID"
#define WIFI_PASS "PASS"

namespace WiFiTools{
    void connectWifi();
    String send_request(const char*);
}