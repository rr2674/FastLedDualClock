#include <Arduino.h>
#include <FastLED.h>

#include "AppManager.h"
#include "Button.h"
#include "IndicatorLED.h"
#include "MovingPixelDemo.h"
#include "DigitDisplayDemo.h"
#include "DisplayModel.h"
#include "DualClock.h"

// --- Configuration ---
#define CHANGE_APP_BUTTON1_PIN 27

#define BUTTON2_PIN 26  // controls: dualclock color, moving pixel color, digit display speed
#define BUTTON3_PIN 25  // dualclock display time or day, moving pixel speed
#define BUTTON4_PIN 33  // dualclock time 24hr or 12hr
#define LED_STRIP_DATA_PIN 5
#define LED_INDICATOR_PIN 21

#define LED_EVENT_BLINK_MS 300

#define NUM_LEDS 120
#define LED_TYPE WS2811
#define COLOR_ORDER BRG

//a 0-255 value for how much to scale all leds before writing them out
#define LED_BRIGHTNESS_HIGH 64   //25% brightness
#define LED_BRIGHTNESS_LOW  20   // 8% brightness

CRGB leds[NUM_LEDS];

Button appButton(CHANGE_APP_BUTTON1_PIN);
AppManager appManager;

Button button2(BUTTON2_PIN);
Button button3(BUTTON3_PIN);
Button button4(BUTTON4_PIN);

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
    statusLED.startSetupBlink();

    FastLED.addLeds<LED_TYPE, LED_STRIP_DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(LED_BRIGHTNESS_HIGH);

    pixelDemo.begin(leds, NUM_LEDS);
    digitDemo.begin(leds, NUM_LEDS);
    dualClock.begin(leds, NUM_LEDS);

    statusLED.stopSetupBlink();
    statusLED.setSolid(true);

}

void loop() {

    //todo: watch for chip time drift... resync to ntp every Sunday?

    // --- Adjust brightness based on time ---
    int hour = dualClock.getHour();
    uint8_t newBrightness = (hour >= 21 || hour < 7) ? LED_BRIGHTNESS_LOW : LED_BRIGHTNESS_HIGH;

    if (newBrightness != lastBrightness) {
        FastLED.setBrightness(newBrightness);
        lastBrightness = newBrightness;
    }

    if (appButton.pressed()) {

        statusLED.blinkEvent(LED_EVENT_BLINK_MS);
        appManager.switchMode();

        pixelDemo.reset();
        digitDemo.reset();

        FastLED.clear();
        FastLED.show();

    }

    if (button2.pressed()) {
        statusLED.blinkEvent(LED_EVENT_BLINK_MS);
        pixelDemo.switchLEDColor();
        dualClock.switchLEDColor();
        digitDemo.setHoldTime();
        
    }

    if (button3.pressed()) {
        statusLED.blinkEvent(LED_EVENT_BLINK_MS);
        dualClock.switchMode();
        pixelDemo.setSpeed();
    }

    if (button4.pressed()) {
        statusLED.blinkEvent(LED_EVENT_BLINK_MS);
        dualClock.switchHourFormat();
    }

    switch (appManager.getMode()) {
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
