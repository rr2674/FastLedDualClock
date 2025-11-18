#pragma once
#include <FastLED.h>

//todo: let's make a namespace for this
enum DisplayElementType {
    DIGIT,
    COLON,
    DASH
};

struct Element {
    const uint8_t segments;  // number of logical segments
    const uint8_t pixels;    // number of pixels per segment
};

static const Element digit = { 7, 4 };
static const Element colon = { 1, 1 };
static const Element dash  = { 1, 2 };

inline const Element& getElementShape(DisplayElementType type) {
    switch (type) {
        case DIGIT: return digit;
        case COLON: return colon;
        case DASH:  return dash;
    }
    return digit; // shouldn't happen
}

struct DisplayElement {
    const char* name;        // "min_ones", "colon", "day_ones", "dash", etc.
    DisplayElementType type; // type of element
    int offset;              // starting location of this element in the LED strip
    CRGB color;
};

static constexpr DisplayElement timeDisplay[] = {
    { "min_ones",     DIGIT,  0, CRGB::Red },
    { "min_tens",     DIGIT, 29, CRGB::Green },
    { "colon_bottom", COLON, 58, CRGB::Yellow },
    { "colon_top",    COLON, 61, CRGB::Yellow },
    { "hour_ones",    DIGIT, 63, CRGB::Blue },
    { "hour_tens",    DIGIT, 92, CRGB::White }
};

static const int NUM_TIME_DISPLAY_ELEMENTS = sizeof(timeDisplay) / sizeof(timeDisplay[0]);

// A–G segment mapping for digits 0–9
static const uint8_t digitSegmentMap[10][7] = {
  // A,B,C,D,E,F,G
  {1,1,1,0,1,1,1}, // 0
  {1,0,0,0,1,0,0}, // 1
  {1,1,0,1,0,1,1}, // 2
  {1,1,0,1,1,1,0}, // 3
  {1,0,1,1,1,0,0}, // 4
  {0,1,1,1,1,1,0}, // 5
  {0,1,1,1,1,1,1}, // 6
  {1,1,0,0,1,0,0}, // 7
  {1,1,1,1,1,1,1}, // 8
  {1,1,1,1,1,0,0}  // 9
};