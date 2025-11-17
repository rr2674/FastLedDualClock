#include "MovingPixelDemo.h"

void MovingPixelDemo::begin(CRGB* leds_, int count) {
    this->leds = leds_;
    this->numLeds = count;
    reset();
}

void MovingPixelDemo::reset() {
    this->pos = 0;
    this->lastMove = millis();
}

void MovingPixelDemo::update() {
    if (millis() - this->lastMove >= this->speedMs) {
        this->lastMove = millis();
        this->pos = (this->pos + 1) % this->numLeds;

        FastLED.clear();
        leds[this->pos] = CRGB::White;
        FastLED.show();
    }
}
