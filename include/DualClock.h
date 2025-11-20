#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ezTime.h>
#include <ArduinoJson.h>

#include "DisplayModel.h"
#include "ColorManager.h"
#include "DualClockModeManager.h"


class DualClock {
public:
    DualClock(const char* ssid, const char* password);

    void begin(CRGB* leds_, int numLeds_);
    void update();
    void reset();
    // Returns the current hour in 24-hour format (0–23)
    int getHour(bool as24hr = true) const;

    void switchHourFormat();
    void setHourFormat(bool use24hr);
    bool is24HourFormat() const { return use24Hour; }

    void switchMode() {
        modeManager.next();
    };
    void switchLEDColor() {
        colorManager.next();
    }

    CRGB getLEDColor() const {
        return colorManager.getColor();
    }

private:
    bool debug = false;

    CRGB* leds = nullptr;

    const char* wifiSSID;
    const char* wifiPassword;

    Timezone tz;
    bool use24Hour = false;   // default = 12-hour mode

    DualClockModeManager  modeManager;
    ColorManager colorManager;

    unsigned long lastUpdate = 0; //Timing variable, we only want to update() to do something once every second

    // use HTTP to sync with NTP servers
    bool syncTimeHTTP();

    void displayTime();
    void displayDate();

    void displayDigit(int digit, int offset);
    void renderDigitElement(const DisplayElement& el, int number);
    void renderColonOrDash(const DisplayElement& el, bool on);

};
