#include "wifi.h"

namespace WiFiTools{
    void connectWifi(){
        delay(2000);
        Serial.println();
        WiFi.begin(WIFI_SSID, WIFI_PASS);
        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
            if (millis() > 15000) ESP.restart();
        }
        Serial.println("Connected");
    }

    String send_request(const char* url) {
        std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
        client->setInsecure();
        HTTPClient https;
        String result;
        if (https.begin(*client, url)) {
            int code = https.GET();
            if (code > 0) {
                result = https.getString();
            }
            else {
                result = ERROR_STR;
                Serial.print("[HTTPS] GET request failed with code: "); Serial.print(code); Serial.print("and url: "); Serial.println(url);
            }
            https.end();
        }
        else{
            result = ERROR_STR;
            Serial.println("Error connection");
        }
        return result;
    }

}