#include <Arduino.h>

// Led strip
#define LED_PIN D4 // Wemos d1 mini - d4
#define WIDTH 16
#define HEIGHT 16
#define LED_COUNT WIDTH *HEIGHT

#define VOLTS 5
#define MILLIAMPS 2500 // 5v 3A

#define EFFECTS_PERIOD 100 // ms
#define REQUEST_PERIOD 300000

 #define DEBUG // Дебаг

// Libraries
#include "ArduinoJson.h"
#include "Timer.h"
#include "wifi.h"
#include "FastLED.h"
#include "NeoMatrix.h"
Timer EffectTmr(EFFECTS_PERIOD);
Timer RequestTmr(REQUEST_PERIOD);
CRGB leds[LED_COUNT];
Matrix<WIDTH, HEIGHT> mx(FastLED, leds);

enum EffectList
{
    RAINBOW,
    RAIN,
    SNOW,
    DRIZZLE,
    CLEAR,
    ATMOSPHERE,
    CLOUDS,
    THUNDERSTORM
};

namespace Effects
{
    void rainbow();
    void snow_falling();
    void rain();
    void thunderstorm();
    void clouds();
    void atmosphere();
    void drizzle();
    void clear_air();

    void objects_fall(CRGB);
}

void requestWeather();
void effectTick();
EffectList effect_num = RAINBOW;

void setup()
{
    // Serial
    Serial.begin(115200);
    // Led strip
    FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, LED_COUNT);
    FastLED.setMaxPowerInVoltsAndMilliamps(VOLTS, MILLIAMPS);
    FastLED.setBrightness(100);
    FastLED.clear();
    FastLED.show();

    // Wifi
    WiFiTools::connectWifi();

    EffectTmr.attach(effectTick);
    RequestTmr.attach(requestWeather);

    // First request
    requestWeather();
}

void loop()
{
    EffectTmr.tick();
    RequestTmr.tick();
}

void requestWeather()
{
    #ifdef DEBUG
    Serial.printf("[%d:%d] ---> Sending request with %s url....\n", millis()/1000, millis()%1000, URL);
    #endif
    String result = WiFiTools::send_request(URL);
    static unsigned char num_bad_request = 0;
    if (result == ERROR_STR)
    {
        effect_num = RAINBOW;
        #ifdef DEBUG
        Serial.printf("[%d:%d] ---> Some error in request. Quantity of bad requests: %d\n Effect: Rainbow\n", millis()/1000, millis()%1000, num_bad_request);
        #endif
        num_bad_request++;
        if (num_bad_request == MAX_BAD_REQUESTS)
        {
            #ifdef DEBUG
            Serial.printf("[%d:%d] ---> Max count of bad request [%d] achived... Effect: Rainbow\n", millis()/1000, millis()%1000, MAX_BAD_REQUESTS);
            #endif
            num_bad_request = 0;
            effect_num = RAINBOW;
        }
        return;
    }
    StaticJsonDocument<1000> doc;
    deserializeJson(doc, result);
    #ifdef DEBUG
    Serial.printf("[%d:%d] ---> Recieve json... Data: %s\n", millis()/1000, millis()%1000, result.c_str());
    #endif
    if (doc["weather"][0]["main"] == "Rain")
        effect_num = RAIN;
    else if (doc["weather"][0]["main"] == "Snow")
        effect_num = SNOW;
    else if (doc["weather"][0]["main"] == "Thunderstorm")
        effect_num = THUNDERSTORM;
    else if (doc["weather"][0]["main"] == "Drizzle")
        effect_num = DRIZZLE;
    else if (doc["weather"][0]["main"] == "Clear")
        effect_num = CLEAR;
    else if (doc["weather"][0]["main"] == "Clouds")
        effect_num = CLOUDS;
    else if (doc["weather"][0]["main"] == "Atmosphere")
        effect_num = ATMOSPHERE;
    #ifdef DEBUG
    else
        Serial.printf("[%d:%d] ---> Unknow weather... Got weather from \"api.openweathermap.org\"; Recieve weather: %s\n", millis()/1000, millis()%1000, String(doc["weather"][0]["main"]).c_str());
    #endif
    FastLED.clear(true);
}

void effectTick()
{
    if (effect_num == DRIZZLE)
        EffectTmr.setTimeout(50);
    else
        EffectTmr.setTimeout(100);
    switch (effect_num)
    {
        case RAINBOW:
            Effects::rainbow();
            break;
        case RAIN:
            Effects::rain();
            break;
        case SNOW:
            Effects::snow_falling();
            break;
        case THUNDERSTORM:
            Effects::thunderstorm();
            break;
        case DRIZZLE:
            Effects::drizzle();
            break;
        case CLEAR:
            Effects::clear_air();
            break;
        case CLOUDS:
            Effects::clouds();
            break;
        case ATMOSPHERE:
            Effects::atmosphere();
            break;
    }
}

namespace Effects
{
    //
    void rainbow()
    {
        static uint8_t coloroffset = 0;
        for (int i = 0; i < WIDTH; i++)
        {
            CHSV thisColor = CHSV(float(255.0 / WIDTH) * i + coloroffset, 255, 255);
            coloroffset++;
            for (int j = 0; j < HEIGHT; j++)
                mx.setPixel(i, j, thisColor);
        }
        FastLED.show();
    }

    void snow_falling()
    {
         Effects::objects_fall(CRGB::White);
    }

    void rain()
    {
         Effects::objects_fall(CRGB::Blue);
    }

    void thunderstorm()
    {
        static uint32_t next_time = 0;
        static uint32_t last_thunder = 0;
        if (millis() - last_thunder >= next_time){
            last_thunder = millis();
            next_time = random(1000, 10000);
            // Thunder code
            mx.fillAll(CRGB::White);
            delay(200);
            FastLED.clear();
            FastLED.show();
        }
    }

    void clouds()
    {
        mx.fillAll(CRGB(0x0000DD));
        for (int i = 0; i < LED_COUNT; i++)
            leds[i].fadeLightBy(180);
        static float pos[] = {5/2, 5/2, 5/2};
        static int posY[] = {7, 2, 13};
        static float vel[3];
        for (int i = 0; i < 3; i++)
            vel[i] = (float)random(1, 20) / 10.0;

        for (int numClous = 0; numClous < 3; numClous++) {
            pos[numClous] += vel[numClous];
            if (pos[numClous] >= WIDTH - 5 / 2) pos[numClous] = 5/2;
            int posx = int(pos[numClous]);
            int posy = int(posY[numClous]);
            mx.setPixel(posx, posy, CRGB::White);
            for (int i = 0; i < 5; i++)
                for (int j = 0; j < 3; j++){
                    uint8_t x = posx - 5/2 + i;
                    uint8_t y = posy - 3/2 + j;
//                    byte bri = constrain((int)255-(255.0/3.601/3)*(sqrt(sq(posx-x) + sq(posy-y)))-random(10), 0, 255);
                    byte bri = 80;
                    CRGB col = CRGB(bri-30, bri-30, constrain((int)bri+30, 0, 255));
                    mx.setPixel(x, y, col);
                }
        }
        FastLED.show();
    }

    void atmosphere()
    {
        for (int i = 0; i < LED_COUNT; i++)
            leds[i] = random(10, 100);
        for (int i = 0; i < LED_COUNT; i++)
            leds[i].fadeLightBy(60);
        FastLED.show();
    }

    void drizzle()
    {
        Effects::objects_fall(CRGB(0x8cfff4));
    }

    void clear_air()
    {
        mx.fillAll(CRGB(0x00bbff));
        FastLED.show();
    }

    void objects_fall(CRGB col)
    {
        for (int i = 0; i < WIDTH; i++)
            for (int j = 0; j < HEIGHT - 1; j++)
                mx.setPixel(i, j, mx.getPixelColor(i, j + 1));

        for (int i = 0; i < WIDTH; i++)
        {
            if (mx.getPixelColor(i, HEIGHT - 2) == 0 && !random(0, 10))
                mx.setPixel(i, HEIGHT - 1, col);
            else
                mx.setPixel(i, HEIGHT - 1, CRGB::Black);
        }
    }
}
