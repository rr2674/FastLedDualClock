#include "DigitDisplayDemo.h"

//todo: let's eliminate globals,make everything instance-based
extern const int NUM_TIME_ELEMENTS;

void DigitDisplayDemo::begin(CRGB* leds_, int numLeds_) {
    this->leds = leds_;

    if (!validateLayout(numLeds_)) {
        Serial.println("ERROR: LED strip is too short for DigitDisplayDemo!");
    }

    this->reset();

#if defined(DEBUG_MODE)
    this->debug = true;
    Serial.println("DigitDisplayDemo initialized in DEBUG mode");
#endif
}

bool DigitDisplayDemo::validateLayout(int numLeds) {
    int requiredPixels = 0;

    for (int i = 0; i < NUM_TIME_DISPLAY_ELEMENTS; i++) {
        const DisplayElement& el = timeDisplay[i];
        const Element& shape = getElementShape(el.type);

        int elementPixels = el.offset + (shape.segments * shape.pixels);

        if (elementPixels > requiredPixels) {
            requiredPixels = elementPixels;
        }
    }

    if (requiredPixels > numLeds) {
        Serial.printf("DigitDisplayDemo layout requires %d LEDs, but only %d available\n",
                      requiredPixels, numLeds);
        return false;
    }

    return true;
}

void DigitDisplayDemo::reset() {
    this->lastUpdate = 0;
    this->currentNumber = -1;
    Serial.println("DigitDisplayDemo reset");
}

void DigitDisplayDemo::update() {

    if (millis() - this->lastUpdate >= this->holdTime) {
        this->lastUpdate = millis();

        // Increment test number 0..9
        this->currentNumber = (this->currentNumber + 1) % 10;

        FastLED.clear();

        // Render every element in the time display
        for (int i = 0; i < NUM_TIME_DISPLAY_ELEMENTS; i++) {

            const DisplayElement& el = timeDisplay[i];

            switch (el.type) {
                case DIGIT:
                    renderDigitElement(el, this->currentNumber);
                    break;

                case COLON:
                case DASH:
                    renderColonOrDash(el);
                    break;
            }
        }

        FastLED.show();
    }
}

void DigitDisplayDemo::renderDigitElement(const DisplayElement& el, int number) {
    const Element& shape = getElementShape(el.type);

    for (int seg = 0; seg < shape.segments; seg++) {
        bool isOn = digitSegmentMap[number][seg];
        CRGB segColor = isOn ? el.color  : CRGB::Black;

        int segmentStart = el.offset + (seg * shape.pixels);
        fill_solid(&leds[segmentStart], shape.pixels, segColor);
    }

    if (this->debug) {
        Serial.printf("Rendered %s digit %d at offset %d\n", el.name, number, el.offset);
    }
}

void DigitDisplayDemo::renderColonOrDash(const DisplayElement& el) {
    const Element& shape = getElementShape(el.type);

    for (int seg = 0; seg < shape.segments; seg++) {
        int segmentStart = el.offset + (seg * shape.pixels);
        fill_solid(&leds[segmentStart], shape.pixels, el.color);
    }

    if (this->debug) {
        Serial.printf("Rendered %s at offset %d\n", el.name, el.offset);
    }
}
