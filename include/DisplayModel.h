#pragma once
#include <FastLED.h>
#include <cstddef>

enum class DisplayElementType {
    DIGIT,
    COLON,
    DASH
};

enum class DigitRole {
    NONE,       // colon, dash, etc.
    ONES_1,     // first pair (minutes, day)
    TENS_1,
    ONES_2,     // second pair (hours, month)
    TENS_2
};

struct Element {
    uint8_t segments;  // number of logical segments
    uint8_t pixels;    // number of pixels per segment
};

struct DisplayElement {
    const char* name;        // "min_ones", "colon", "day_ones", "dash", etc.
    DisplayElementType type;
    DigitRole role;
    int offset;              // starting location of this element in the LED strip
    CRGB color;
};

class DisplayModel {
public:
    DisplayModel() = delete; // all static, no instances

    // Accessors for display tables
    static const DisplayElement* getTimeDisplay();
    static size_t getTimeDisplayCount();

    static const DisplayElement* getDateDisplay();
    static size_t getDateDisplayCount();

    // Accessor for digit segment map
    static const uint8_t (*getDigitSegmentMap())[7];  // 7 segments to make a digit
    static size_t getDigitCount();

    // Accessor for shape info
    static const Element& getElementShape(DisplayElementType type);

    // Helper to compute digit value from role
    static int computeDigit(DigitRole role, int primary, int secondary);

    static bool validateLayout(int numLeds);

private:
    static const Element digit;
    static const Element colon;
    static const Element dash;

    static const DisplayElement timeDisplay[];
    static const DisplayElement dateDisplay[];
    static const uint8_t digitSegmentMap[][7]; // 7 segments to make a digit

};
