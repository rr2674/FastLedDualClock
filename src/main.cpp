#include <Arduino.h>
#include <FastLED.h>
#include <WiFi.h>
#include <esp_wifi.h>

#include "AppManager.h"
#include "Button.h"
#include "IndicatorLED.h"
#include "MovingPixelDemo.h"
#include "DigitDisplayDemo.h"
#include "DisplayModel.h"
#include "DualClock.h"

// --- Configuration ---
#define CHANGE_APP_BUTTON1_PIN 27

#define BUTTON2_PIN 26  // controls: dualclock color, moving pixel color, digit display  number
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
DigitDisplayDemo digitDemo(DigitDisplayDemo::Mode::STEP_ROTATE);

// WiFi credentials from build flags
const char* WIFI_SSID = WIFI_SSID_OVERRIDE;
const char* WIFI_PASSWORD = WIFI_PASSWORD_OVERRIDE;

DualClock dualClock(WIFI_SSID, WIFI_PASSWORD, "America/Chicago");

uint8_t lastBrightness = 0;

/*
** helper function prototypes
*/
void spoof_mac_address();
bool parseMac(const char* str, uint8_t* mac);


void setup() {

    Serial.begin(115200);
    delay(1000);

#ifdef WIFI_MAC_STR
    spoof_mac_address();
#endif

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

    // --- Adjust brightness based on time ---
    int hour = dualClock.getHour();
    uint8_t newBrightness = (hour >= 21 || hour < 7) ? LED_BRIGHTNESS_LOW : LED_BRIGHTNESS_HIGH;

    if (newBrightness != lastBrightness) {
        FastLED.setBrightness(newBrightness);
        lastBrightness = newBrightness;
    }

    dualClock.checkWiFi();      // reconnect to WiFi if needed, or reboot esp if WiFi down for > 30 minutes

    if (appButton.pressed()) {

        statusLED.blinkEvent(LED_EVENT_BLINK_MS);
        appManager.switchApp();

        pixelDemo.reset();
        digitDemo.reset();

        FastLED.clear();
        FastLED.show();

    }

    if (button2.pressed()) {
        statusLED.blinkEvent(LED_EVENT_BLINK_MS);
        pixelDemo.switchLEDColor();
        dualClock.switchLEDColor();
        //digitDemo.setHoldTime();
        digitDemo.nextNumber();  // manually advance number instead of changing the hold time
        
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

    switch (appManager.getApp()) {
        case AppManager::AppName::PIXEL:
            pixelDemo.update();
            break;

        case AppManager::AppName::DIGITS:
            digitDemo.update();
            break;
        
        case AppManager::AppName::DUALCLOCK:
            dualClock.update();
            break;
    }

    statusLED.update();

}

/* 
* helper functions
*/

// Convert "DE:AD:BE:EF:CA:FE" → {0xDE,0xAD,...}
bool parseMac(const char* str, uint8_t* mac) {
    return sscanf(str, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
                  &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]) == 6;
}

void spoof_mac_address() {

    Serial.print("Original MAC is: ");
    Serial.println(WiFi.macAddress());

    // Disable WiFi before changing MAC
    WiFi.mode(WIFI_OFF);
    delay(100);
    esp_wifi_restore();   // clears WiFi config (NVS)
    delay(50);

    uint8_t customMAC[6];
    if (!parseMac(WIFI_MAC_STR, customMAC)) {
        Serial.println("Invalid MAC string format!");
        return;
    }

    Serial.print("Changing MAC to: ");
    Serial.println(WIFI_MAC_STR);

    esp_err_t err = esp_wifi_set_mac(WIFI_IF_STA, customMAC);
    if (err != ESP_OK) {
        Serial.printf("esp_wifi_set_mac() FAILED, error = %d\n", err);
    }

    // Re-enable WiFi AFTER setting MAC
    WiFi.mode(WIFI_STA);

    Serial.print("Active MAC is: ");
    Serial.println(WiFi.macAddress());
}


