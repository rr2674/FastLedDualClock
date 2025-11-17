// MovingPixelDemo.h
#pragma once
#include <FastLED.h>

class MovingPixelDemo {
public:
    void begin(CRGB* leds_, int count);
    void update();
    void reset();
private:
    CRGB* leds;
    int numLeds;
    int pos = 0;
    uint32_t lastMove = 0;
    const uint32_t speedMs = 40;
};
