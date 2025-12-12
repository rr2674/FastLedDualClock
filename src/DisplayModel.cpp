#include "DisplayModel.h"

// ----------------------
// Shapes
// ----------------------
const DisplayModel::Element DisplayModel::digit = { 7, 4 };
const DisplayModel::Element DisplayModel::colon = { 1, 1 };
const DisplayModel::Element DisplayModel::dash  = { 1, 1 };

// ----------------------
// Time table
// ----------------------
const DisplayModel::DisplayElement DisplayModel::timeDisplay[6] = {
    { "min_ones",     DisplayElementType::DIGIT, DigitRole::ONES_1,  0, CRGB::Red    },
    { "min_tens",     DisplayElementType::DIGIT, DigitRole::TENS_1, 29, CRGB::Green  },
    { "colon_bottom", DisplayElementType::COLON, DigitRole::NONE,   58, CRGB::Yellow },
    { "colon_top",    DisplayElementType::COLON, DigitRole::NONE,   61, CRGB::Yellow },
    { "hour_ones",    DisplayElementType::DIGIT, DigitRole::ONES_2, 63, CRGB::Blue   },
    { "hour_tens",    DisplayElementType::DIGIT, DigitRole::TENS_2, 92, CRGB::White  }
};

// ----------------------
// Date table
// ----------------------
const DisplayModel::DisplayElement DisplayModel::dateDisplay[6] = {
    { "day_ones",     DisplayElementType::DIGIT, DigitRole::ONES_1,  0, CRGB::Red    },
    { "day_tens",     DisplayElementType::DIGIT, DigitRole::TENS_1, 29, CRGB::Green  },
    { "dash_right",   DisplayElementType::DASH,  DigitRole::NONE,   57, CRGB::Yellow },
    { "dash_left",    DisplayElementType::DASH,  DigitRole::NONE,   62, CRGB::Yellow },
    { "month_ones",   DisplayElementType::DIGIT, DigitRole::ONES_2, 63, CRGB::Blue   },
    { "month_tens",   DisplayElementType::DIGIT, DigitRole::TENS_2, 92, CRGB::White  }
};

// ----------------------
// Digit segment map (A-G)
// control which segment pixels are 'on' for a number 0-9
// ----------------------
const uint8_t DisplayModel::digitSegmentMap[10][7] = {
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

// ----------------------
// Accessors for time/date tables
// ----------------------
const DisplayModel::DisplayElement* DisplayModel::getTimeDisplay() { return timeDisplay; }
size_t DisplayModel::getTimeDisplayCount() { return sizeof(timeDisplay)/sizeof(timeDisplay[0]); }

const DisplayModel::DisplayElement* DisplayModel::getDateDisplay() { return dateDisplay; }
size_t DisplayModel::getDateDisplayCount() { return sizeof(dateDisplay)/sizeof(dateDisplay[0]); }

// ----------------------
// Accessors for digit segment map
// ----------------------
const uint8_t (*DisplayModel::getDigitSegmentMap())[7] { return digitSegmentMap; }
size_t DisplayModel::getDigitCount() { return sizeof(digitSegmentMap)/sizeof(digitSegmentMap[0]); }

// ----------------------
// Accessor for shape info
// ----------------------
const DisplayModel::Element& DisplayModel::getElementShape(DisplayElementType type) {
    switch(type) {
        case DisplayElementType::DIGIT: return digit;
        case DisplayElementType::COLON: return colon;
        case DisplayElementType::DASH:  return dash;
        default: return digit;
    }
}

bool DisplayModel::validateLayout(int numLeds) {
    int requiredPixels = 0;

    // ----- Check time display -----
    for (size_t i = 0; i < getTimeDisplayCount(); i++) {
        const DisplayElement& el = timeDisplay[i];
        const Element& shape = getElementShape(el.type);

        int elementPixels = el.offset + (shape.segments * shape.pixels);
        if (elementPixels > requiredPixels) {
            requiredPixels = elementPixels;
        }
    }

    // ----- Check date display -----
    for (size_t i = 0; i < getDateDisplayCount(); i++) {
        const DisplayElement& el = dateDisplay[i];
        const Element& shape = getElementShape(el.type);

        int elementPixels = el.offset + (shape.segments * shape.pixels);
        if (elementPixels > requiredPixels) {
            requiredPixels = elementPixels;
        }
    }

#ifdef DEBUG_MODE
    Serial.printf("DisplayModel::validateLayout() requiredPixels=%d, available=%d\n",
                  requiredPixels, numLeds);
#endif

    return requiredPixels <= numLeds;
}

int DisplayModel::computeDigit(DigitRole role, int primary, int secondary) {
    switch (role) {
        case DigitRole::ONES_1: return primary % 10;
        case DigitRole::TENS_1: return primary / 10;
        case DigitRole::ONES_2: return secondary % 10;
        case DigitRole::TENS_2: return secondary / 10;
        case DigitRole::NONE:
        default: return -1;
    }
}
