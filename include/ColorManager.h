#pragma once
#include <FastLED.h>

class ColorManager {
public:
    ColorManager() : index(0) {}

    struct ColorInfo {
        const char* name;
        CRGB        value;
    };

    CRGB getColor() const;
    const char* getName() const;
    void next();

    uint8_t getIndex() const { return index; }
    static constexpr uint8_t COUNT = 8;

private:
    uint8_t index;
    static const ColorInfo table[COUNT];
};
