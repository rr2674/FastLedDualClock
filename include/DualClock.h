#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ezTime.h>
#include <ArduinoJson.h>

#include "DisplayModel.h"
#include "ColorManager.h"

//todo: make this typedef and array?
enum class DisplayMode : uint8_t {
    TIME,
    DATE,
    DISPLAY_MODE_COUNT  // always keep this last
};


class DualClock {
public:
    DualClock(const char* ssid, const char* password);

    void begin(CRGB* leds_, int numLeds_);
    void update();
    void reset();
    
    void switchMode();
    void switchLEDColor() {
        colorManager.next();
    }

    CRGB getLEDColor() const {
        return colorManager.getColor();
    }

private:
    bool debug = false;

    CRGB* leds;

    const char* wifiSSID;
    const char* wifiPassword;

    Timezone tz;

    DisplayMode  currentMode;
    ColorManager colorManager;

    unsigned long lastUpdate = 0; //Timing variable, we only want to update() to do something once every second

    // use HTTP to sync with NTP servers
    bool syncTimeHTTP();
    bool validateLayout(int numLeds);

    void displayTime();
    void displayDate();

    void displayDigit(int digit, int offset);
    void renderDigitElement(const DisplayElement& el, int number);
    void renderColonOrDash(const DisplayElement& el, bool on);

    const char* modeToString(DisplayMode m) const {
        switch (m) {
            case DisplayMode::TIME:    return "TIME";
            case DisplayMode::DATE:    return "DATE";
            default:      return "UNKNOWN";
        }
    }

};
