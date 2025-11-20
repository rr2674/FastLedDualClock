#include <Arduino.h>
#include <FastLED.h>

#include "ModeManager.h"
#include "Button.h"
#include "IndicatorLED.h"
#include "MovingPixelDemo.h"
#include "DigitDisplayDemo.h"
#include "DisplayModel.h"
#include "DualClock.h"

// --- Configuration ---
#define CHANGE_APP_BUTTON_PIN 27
#define CHANGE_COLOR_BUTTON_PIN 26
#define CHANGE_DATE_TIME_BUTTON_PIN 25
//#define CHANGE_12HR_24HR_BUTTON_PIN ?
#define LED_STRIP_DATA_PIN 5
#define LED_INDICATOR_PIN 2

#define LED_EVENT_BLINK_MS 300

#define NUM_LEDS 120
#define LED_TYPE WS2811
#define COLOR_ORDER BRG

#define LED_BRIGHTNESS_HIGH 64   //a 0-255 value for how much to scale all leds before writing them out
#define LED_BRIGHTNESS_LOW  20

CRGB leds[NUM_LEDS];

Button modeButton(CHANGE_APP_BUTTON_PIN);
ModeManager modeManager;

Button colorButton(CHANGE_COLOR_BUTTON_PIN);
Button dateTimeButton(CHANGE_DATE_TIME_BUTTON_PIN);

IndicatorLED statusLED(LED_INDICATOR_PIN);

MovingPixelDemo pixelDemo;
DigitDisplayDemo digitDemo;

// WiFi credentials from build flags
const char* WIFI_SSID = WIFI_SSID_OVERRIDE;
const char* WIFI_PASSWORD = WIFI_PASSWORD_OVERRIDE;

DualClock dualClock(WIFI_SSID, WIFI_PASSWORD);

uint8_t lastBrightness = 0;

void setup() {

    Serial.begin(115200);
    delay(1000);

    statusLED.begin();

    FastLED.addLeds<LED_TYPE, LED_STRIP_DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(LED_BRIGHTNESS_HIGH);

    pixelDemo.begin(leds, NUM_LEDS);
    digitDemo.begin(leds, NUM_LEDS);
    dualClock.begin(leds, NUM_LEDS);

    statusLED.stopSetupBlink();
    statusLED.setSolid(true);

}

void loop() {

    // --- Adjust brightness based on time ---
    int hour = dualClock.getHour();
    uint8_t newBrightness = (hour >= 21 || hour < 7) ? LED_BRIGHTNESS_LOW : LED_BRIGHTNESS_HIGH;

    if (newBrightness != lastBrightness) {
        FastLED.setBrightness(newBrightness);
        lastBrightness = newBrightness;
    }

    if (modeButton.pressed()) {

        statusLED.blinkEvent(LED_EVENT_BLINK_MS);
        modeManager.switchMode();

        pixelDemo.reset();
        digitDemo.reset();

        FastLED.clear();
        FastLED.show();

    }

    if (colorButton.pressed()) {
        statusLED.blinkEvent(LED_EVENT_BLINK_MS);
        pixelDemo.switchLEDColor();
        dualClock.switchLEDColor();
        
    }

    if (dateTimeButton.pressed()) {
        statusLED.blinkEvent(LED_EVENT_BLINK_MS);
        dualClock.switchMode();
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

    statusLED.update();

}
