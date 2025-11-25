#pragma once
#include <FastLED.h>
#include "DisplayModel.h"

class DigitDisplayDemo {
public:

    enum class Mode {
        AUTO_ROTATE,   // numeric value updates automatically
        STEP_ROTATE    // numeric value only changes on button press
    };

    DigitDisplayDemo(Mode mode)
        : mode(mode),
          holdTime(1*1000),
          currentNumber(0),
          lastUpdate(0),
          shouldRedraw(mode == Mode::STEP_ROTATE)
    {}

    void begin(CRGB* leds_, int numLeds_);
    void update();
    void setHoldTime(unsigned long ms = 0);
    void nextNumber();
    void reset();

private:
    CRGB* leds = nullptr;
    Mode mode;
    bool shouldRedraw;
    unsigned long lastUpdate;
    int currentNumber;
    unsigned long holdTime;

    bool debug = false;

    bool validateLayout(int numLeds);

    void renderDigitElement(const DisplayModel::DisplayElement& el, int number);
    void renderColonOrDash(const DisplayModel::DisplayElement& el);
};
