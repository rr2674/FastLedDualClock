#pragma once
#include <Arduino.h>

class IndicatorLED {
public:
    IndicatorLED(uint8_t pin);

    void begin();

    // Blink for setup in background task
    void startSetupBlink(unsigned long interval = 500);
    void stopSetupBlink();

    void setSolid(bool on);
    void blinkEvent(unsigned long duration = 200);
    void update();

private:
    uint8_t pin;
    bool solidState = false;           // true if solid ON
    bool eventBlinking = false;        // true if we are blinking due to event
    unsigned long blinkInterval = 500; // default blink interval for setup
    unsigned long lastToggle = 0;      // last time we toggled
    unsigned long eventEndTime = 0;    // when event blink ends
    bool ledState = false;             // current state of LED (HIGH/LOW)

    // Task handle
    TaskHandle_t blinkTaskHandle = nullptr;
    static void blinkTask(void* param); // FreeRTOS task function
};
