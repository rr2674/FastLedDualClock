#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ezTime.h>
#include <ArduinoJson.h>

#include "DisplayModel.h"

enum class DisplayMode : uint8_t {
    TIME,
    DATE,
    MODE_COUNT  // always keep this last
};

class DualityClock {
public:
    DualityClock(const char* ssid, const char* password);

    void begin(CRGB* leds_, int numLeds_);
    void update();

    
    void switchMode();
    void setLEDColor(RgbColor color);

private:
    bool debug = false;

    CRGB* leds;

    const char* wifiSSID;
    const char* wifiPassword;

    DisplayMode displayMode;
    Timezone tz;

    RgbColor ledColor;

    unsigned long lastUpdate = 0; //Timing variable, we only want to update() to do something once every second
    unsigned long lastMinute = -1; // for detecing changes, we only want to update when time or date changes

    // use HTTP to sync with NTP servers
    bool syncTimeHTTP();

    void displayTime();
    void displayDate();

    void displayDigit(int digit, int offset);
    void renderDigitElement(const DisplayElement& el, int number);
    void renderColonOrDash(const DisplayElement& el, bool on);

    const char* modeToString(Mode m) const {
        switch (m) {
            case TIME:    return "TIME";
            case DATE:    return "DATE";
            default:      return "UNKNOWN";
        }
    }
};
