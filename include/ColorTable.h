#pragma once
#include <FastLED.h>

//todo: should we name space this?

struct ColorInfo {
    const char* name;
    CRGB        ledColor;
};

static constexpr ColorInfo colorTable[] = {
    { "Red",     CRGB::Red },
    { "Green",   CRGB::Green },
    { "Blue",    CRGB::Blue },
    { "White",   CRGB::White },
    { "Yellow",  CRGB::Yellow },
    { "Orange",  CRGB::Orange },
    { "Cyan",    CRGB::Cyan },
    { "Magenta", CRGB::Magenta }
};

static constexpr uint8_t COLOR_COUNT =
    sizeof(colorTable) / sizeof(colorTable[0]);
