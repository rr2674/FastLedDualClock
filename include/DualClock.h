#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ezTime.h>
#include <ArduinoJson.h>

#include "DisplayModel.h"
#include "ColorTable.h"

//todo: make this typedef and array?
enum class DisplayMode : uint8_t {
    TIME,
    DATE,
    MODE_COUNT  // always keep this last
};


class DualClock {
public:
    DualClock(const char* ssid, const char* password);

    void begin(CRGB* leds_, int numLeds_);
    void update();

    void switchMode();
    void switchLEDColor();

    //todo: why here and not in .cpp file?
    CRGB getColor() const {
        return colorTable[colorIndex].ledColor;
    }

private:
    bool debug = false;

    CRGB* leds;

    const char* wifiSSID;
    const char* wifiPassword;

    Timezone tz;

    DisplayMode  currentMode;

    uint8_t colorIndex = 0;

    unsigned long lastUpdate = 0; //Timing variable, we only want to update() to do something once every second
    unsigned long lastMinute = -1; // for detecing changes, we only want to update when time or date changes

    // use HTTP to sync with NTP servers
    bool syncTimeHTTP();

    void displayTime();
    void displayDate();

    void displayDigit(int digit, int offset);
    void renderDigitElement(const DisplayElement& el, int number);
    void renderColonOrDash(const DisplayElement& el, bool on);

    const char* modeToString(DisplayMode m) const {
        switch (m) {
            case TIME:    return "TIME";
            case DATE:    return "DATE";
            default:      return "UNKNOWN";
        }
    }


