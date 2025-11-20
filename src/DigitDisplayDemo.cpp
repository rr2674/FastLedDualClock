#include "DigitDisplayDemo.h"

void DigitDisplayDemo::begin(CRGB* leds_, int numLeds_) {
    this->leds = leds_;

    if (!DisplayModel::validateLayout(numLeds_)) {
        Serial.println("ERROR: LED strip is too short for DigitDisplayDemo!");
    }

    this->reset();

#if defined(DEBUG_MODE)
    this->debug = true;
    Serial.println("DigitDisplayDemo initialized in DEBUG mode");
#endif
}

void DigitDisplayDemo::reset() {
    this->lastUpdate = 0;
    this->currentNumber = -1;
    Serial.println("DigitDisplayDemo reset");
}

void DigitDisplayDemo::setHoldTime(unsigned long ms) {
    if (ms == 0) {
        // double speed
        holdTime *= 2;

        // Wrap around if exceeding 16 seconds
        if (holdTime > 16*1000) {
            holdTime = 1*1000;
        }
    } else {
        holdTime = ms;
    }
}

void DigitDisplayDemo::update() {

    if (millis() - this->lastUpdate >= this->holdTime) {
        this->lastUpdate = millis();

        // Increment test number 0..9
        this->currentNumber = (this->currentNumber + 1) % 10;

        FastLED.clear();

        // Loop through all elements in the time display
        for (size_t i = 0; i < DisplayModel::getTimeDisplayCount(); ++i) {
            const auto& el = DisplayModel::getTimeDisplay()[i];

            switch (el.type) {
                case DisplayElementType::DIGIT: {
                    int digit = DisplayModel::computeDigit(el.role, this->currentNumber, this->currentNumber);
                    renderDigitElement(el, digit);
                    break;
                }
                case DisplayElementType::COLON:
                case DisplayElementType::DASH:
                    renderColonOrDash(el);
                    break;
            }
        }

        FastLED.show();
    }
}

void DigitDisplayDemo::renderDigitElement(const DisplayElement& el, int number) {
    const auto& shape = DisplayModel::getElementShape(el.type);
    const auto& map = DisplayModel::getDigitSegmentMap();

    for (uint8_t seg = 0; seg < shape.segments; ++seg) {
        bool isOn = map[number][seg];
        CRGB segColor = isOn ? el.color : CRGB::Black;

        int segmentStart = el.offset + (seg * shape.pixels);
        fill_solid(&leds[segmentStart], shape.pixels, segColor);
    }

    if (this->debug) {
        Serial.printf("Rendered %s digit %d at offset %d\n", el.name, number, el.offset);
    }
}

void DigitDisplayDemo::renderColonOrDash(const DisplayElement& el) {
    const auto& shape = DisplayModel::getElementShape(el.type);

    for (uint8_t seg = 0; seg < shape.segments; ++seg) {
        int segmentStart = el.offset + (seg * shape.pixels);
        fill_solid(&leds[segmentStart], shape.pixels, el.color);
    }

    if (this->debug) {
        Serial.printf("Rendered %s at offset %d\n", el.name, el.offset);
    }
}
