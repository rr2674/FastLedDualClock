#include "DigitDisplayDemo.h"

void DigitDisplayDemo::begin(CRGB* leds_, int numLeds_) {
    leds = leds_;

    if (!DisplayModel::validateLayout(numLeds_)) {
        Serial.println("ERROR: LED strip is too short for DigitDisplayDemo!");
    }

    reset();

#if defined(DEBUG_MODE)
    debug = true;
    Serial.println("DigitDisplayDemo initialized in DEBUG mode");
#endif
}

void DigitDisplayDemo::reset() {
    lastUpdate = 0;
    currentNumber = 0;
    shouldRedraw = true;

    if (debug)  {
        Serial.println("DigitDisplayDemo reset");
    }
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

void  DigitDisplayDemo::nextNumber() {
    currentNumber = (currentNumber + 1) % 10;
    shouldRedraw = true;
}

void DigitDisplayDemo::update() {

    unsigned long now = millis();
    bool triggerUpdate = false;

    if (mode == Mode::AUTO_ROTATE) {
        if (now - lastUpdate >= holdTime) {
            lastUpdate = now;

            // Increment test number 0..9
            currentNumber = (currentNumber + 1) % 10;
            triggerUpdate = true;
        }
    } else {
        // STEP mode: no time-based updates
        if (shouldRedraw) {
            shouldRedraw = false;
            triggerUpdate = true;
        }
    }

    if ( triggerUpdate ) {

        FastLED.clear();

        // Loop through all elements in the time display
        for (size_t i = 0; i < DisplayModel::getTimeDisplayCount(); ++i) {
            const auto& el = DisplayModel::getTimeDisplay()[i];

            switch (el.type) {
                case DisplayModel::DisplayElementType::DIGIT: {
                    renderDigitElement(el, currentNumber);
                    break;
                }
                case DisplayModel::DisplayElementType::COLON:
                case DisplayModel::DisplayElementType::DASH:
                    renderColonOrDash(el);
                    break;
            }
        }

        FastLED.show();
    }
}

void DigitDisplayDemo::renderDigitElement(const DisplayModel::DisplayElement& el, int number) {
    const auto& shape = DisplayModel::getElementShape(el.type);
    const auto& map = DisplayModel::getDigitSegmentMap();

    for (uint8_t seg = 0; seg < shape.segments; ++seg) {
        bool isOn = map[number][seg];
        CRGB segColor = isOn ? el.color : CRGB::Black;

        int segmentStart = el.offset + (seg * shape.pixels);
        fill_solid(&leds[segmentStart], shape.pixels, segColor);
    }

    if (debug) {
        Serial.printf("[DEBUG] Rendered %s digit %d at offset %d\n", el.name, number, el.offset);
    }
}

void DigitDisplayDemo::renderColonOrDash(const DisplayModel::DisplayElement& el) {
    const auto& shape = DisplayModel::getElementShape(el.type);

    for (uint8_t seg = 0; seg < shape.segments; ++seg) {
        int segmentStart = el.offset + (seg * shape.pixels);
        fill_solid(&leds[segmentStart], shape.pixels, el.color);
    }

    if (debug) {
        Serial.printf("[DEBUG] Rendered %s at offset %d\n", el.name, el.offset);
    }
}
