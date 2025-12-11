#include "ColorManager.h"

const ColorManager::ColorInfo ColorManager::table[ColorManager::COUNT] = {
    { "Magenta", CRGB::Magenta },
    { "Red",     CRGB::Red },
    { "Green",   CRGB::Green },
    { "Blue",    CRGB::Blue },
    { "White",   CRGB::White },
    { "Yellow",  CRGB::Yellow },
    { "Orange",  CRGB::Orange },
    { "Cyan",    CRGB::Cyan }
};

CRGB ColorManager::getColor() const {
    return table[index].value;
}

const char* ColorManager::getName() const {
    return table[index].name;
}

void ColorManager::next() {
    index = (index + 1) % COUNT;
}
