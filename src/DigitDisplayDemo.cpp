#include "DigitDisplayDemo.h"

extern const int NUM_TIME_ELEMENTS;
extern DisplayElement timeDisplay[];

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
