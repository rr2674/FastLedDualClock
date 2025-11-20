#pragma once
#include <FastLED.h>
#include "DisplayModel.h"

class DigitDisplayDemo {
public:
    DigitDisplayDemo() = default;

    void begin(CRGB* leds_, int numLeds_);
    void update();
    void reset();
private:
    CRGB* leds = nullptr;
    unsigned long lastUpdate = 0;
    int currentNumber = -1;
    const unsigned long holdTime = 5*1000;

    bool debug = false;

    bool validateLayout(int numLeds);

    void renderDigitElement(const DisplayElement& el, int number);
    void renderColonOrDash(const DisplayElement& el);
};
