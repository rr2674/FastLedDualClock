#pragma once
#include <FastLED.h>
#include "ColorManager.h"

class MovingPixelDemo {
public:
    void begin(CRGB* leds_, int count);

    void update();
    void reset();

    void setSpeed(unsigned long ms) { speedMs = ms; }
    void setTailLength(uint8_t len) { tailLength = len; }

    void switchLEDColor() {
        colorManager.next();
    }

private:
    CRGB* leds = nullptr;
    int numLeds;
    int pos = 0;
    unsigned long lastMove = 0;
    unsigned long speedMs = 100;

    uint8_t tailLength = 5;   // default tail length
    ColorManager colorManager;
};
