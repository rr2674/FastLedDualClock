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

enum DisplayColor : uint8_t {
    COLOR_RED,
    COLOR_GREEN,
    COLOR_BLUE,
    COLOR_WHITE,
    COLOR_YELLOW,
    COLOR_ORANGE,
    COLOR_CYAN,
    COLOR_MAGENTA,
    COLOR_COUNT    // always last, used for cycling
};


class DualityClock {
public:
    DualityClock(const char* ssid, const char* password);

    void begin(CRGB* leds_, int numLeds_);
    void update();

    void switchMode();
    void switchLEDColor();

private:
    bool debug = false;

    CRGB* leds;

    const char* wifiSSID;
    const char* wifiPassword;

    Timezone tz;

    DisplayMode  currentMode;
    DisplayColor currentColor;

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

    const char* colorToString(DisplayColor c) const {
        switch (c) {
            case CCOLOR_RED:    return "Red";
            case COLOR_GREEN:   return "Green";
            case COLOR_BLUE:    return "Blue";
            case COLOR_WHITE:   return "White";
            case COLOR_YELLOW:  return "Yellow";
            case COLOR_ORANGE:  return "Orange";
            case COLOR_CYAN:    return "CYAN";
            case COLOR_MAGENTA: return "Magenta";
            default:            return "UNKNOWN";
        }
    }
};
