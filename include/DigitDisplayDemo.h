#pragma once
#include <FastLED.h>
#include "DisplayModel.h"

class DigitDisplayDemo {
public:
    void begin(CRGB* leds_, int numLeds_);
    void update();
    void reset();
private:
    CRGB* leds;
    int currentNumber = -1;
    uint32_t lastUpdate = 0;
    static const uint32_t holdTime = 5*1000;

    bool debug = false;

    bool validateLayout(int numLeds);
    void renderDigitElement(const DisplayElement& el, int number);
    void renderColonOrDash(const DisplayElement& el);
};
