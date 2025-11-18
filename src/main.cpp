#include <Arduino.h>
#include <FastLED.h>

#include "ModeManager.h"
#include "Button.h"
#include "MovingPixelDemo.h"
#include "DigitDisplayDemo.h"
#include "DisplayModel.h"
#include "DualClock.h"

// --- Configuration ---
#define CHANGE_APP_BUTTON_PIN 26
#define CHANGE_DATE_TIME_COLOR_BUTTON_PIN 25
#define LED_DATA_PIN 5
#define NUM_LEDS 120
#define LED_TYPE WS2811
#define COLOR_ORDER BRG

CRGB leds[NUM_LEDS];

Button modeButton(CHANGE_APP_BUTTON_PIN);
ModeManager modeManager;

Button colorButton(CHANGE_DATE_TIME_COLOR_BUTTON_PIN);

MovingPixelDemo pixelDemo;
DigitDisplayDemo digitDemo;

// WiFi credentials from build flags
const char* WIFI_SSID = WIFI_SSID_OVERRIDE;
const char* WIFI_PASSWORD = WIFI_PASSWORD_OVERRIDE;

DualClock dualClock(WIFI_SSID, WIFI_PASSWORD);

void setup() {
  Serial.begin(115200);
  delay(1000);

  FastLED.addLeds<LED_TYPE, LED_DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(64);

  pixelDemo.begin(leds, NUM_LEDS);
  digitDemo.begin(leds, NUM_LEDS);
  dualClock.begin(leds, NUM_LEDS);

}

void loop() {

    if (modeButton.pressed()) {

        modeManager.switchMode();

        pixelDemo.reset();
        digitDemo.reset();

        FastLED.clear();
        FastLED.show();

    }

    if (colorButton.pressed()) {
        dualClock.switchLEDColor();
    }

    switch (modeManager.getMode()) {
        case Mode::MODE_PIXEL:
            pixelDemo.update();
            break;

        case Mode::MODE_DIGITS:
            digitDemo.update();
            break;
        
        case Mode::MODE_DUALCLOCK:
            dualClock.update();
            break;
    }

}
