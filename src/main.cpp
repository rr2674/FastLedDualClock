#include <Arduino.h>
#include <FastLED.h>

#include "ModeManager.h"
#include "Button.h"
#include "MovingPixelDemo.h"
#include "DigitDisplayDemo.h"
#include "DisplayModel.h"

// --- Configuration ---
#define TEST_MODE_BUTTON_PIN 27
#define LED_DATA_PIN 5
#define NUM_LEDS 120
#define LED_TYPE WS2811
#define COLOR_ORDER BRG

CRGB leds[NUM_LEDS];

// --- Global objects ---
Button modeButton(TEST_MODE_BUTTON_PIN);
ModeManager modeManager;

MovingPixelDemo pixelDemo;
DigitDisplayDemo digitDemo;

void setup() {
  Serial.begin(115200);
  delay(1000);

  FastLED.addLeds<LED_TYPE, LED_DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(64);

  pixelDemo.begin(leds, NUM_LEDS);
  digitDemo.begin(leds, NUM_LEDS);
}

void loop() {

    if (modeButton.pressed()) {

        modeManager.switchMode();

        pixelDemo.reset();
        digitDemo.reset();

        FastLED.clear();
        FastLED.show();

    }

    switch (modeManager.getMode()) {
        case MODE_PIXEL:
            pixelDemo.update();
            break;

        case MODE_DIGITS:
            digitDemo.update();
            break;
    }

}
