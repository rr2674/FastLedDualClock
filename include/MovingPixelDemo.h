#pragma once
#include <FastLED.h>
#include "ColorManager.h"

class MovingPixelDemo {
public:
    void begin(CRGB* leds_, int count);

    void update();
    void reset();

    void setSpeed(unsigned long ms = 0);
    void setTailLength(uint8_t len) { tailLength = len; }

    void switchLEDColor() {
        colorManager.next();
    }

private:
    CRGB* leds = nullptr;
    int numLeds;
    int pos = 0;
    unsigned long lastMove = 0;
    unsigned long speedMs = 20;

    uint8_t tailLength = 5;   // default tail length
    ColorManager colorManager;

    inline void clampToBlack(CRGB &c) {
        //go to black if 10% of current global brightness
        uint8_t threshold = max(1, FastLED.getBrightness() * 10 / 100);
        if (c.r < threshold) c.r = 0;
        if (c.g < threshold) c.g = 0;
        if (c.b < threshold) c.b = 0;
    }

};
