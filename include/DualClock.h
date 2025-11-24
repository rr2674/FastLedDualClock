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
    DualClock(const char* ssid, const char* password, const char* timezone);

    void begin(CRGB* leds_, int numLeds_);
    void update();
    void reset();
    // Returns the current hour in 24-hour format (0–23)
    int getHour(bool as24hr = true) const;

    void switchHourFormat();
    void setHourFormat(bool use24hr);
    bool is24HourFormat() const { return use24Hour; }

    void switchMode(); 

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
    const char* tzName;

    Timezone tz;
    bool use24Hour = false;   // default = 12-hour mode

    DualClockModeManager  modeManager;
    ColorManager colorManager;

    unsigned long lastUpdate = 0; //we only want to update() to do something once every second

    unsigned long lastHttpSync = 0;
    const unsigned long httpSyncInterval = 12UL * 60UL * 60UL * 1000UL; // every 12 hours: hours * minutes * seconds * milliseconds

    bool syncTimeHTTP();

    void displayTime();
    void displayDate();

    void displayDigit(int digit, int offset);
    void renderDigitElement(const DisplayElement& el, int number);
    void renderColonOrDash(const DisplayElement& el, bool on);

};
