#include "IndicatorLED.h"

IndicatorLED::IndicatorLED(uint8_t pin) : pin(pin) {}

void IndicatorLED::begin() {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
    ledState = false;
}

void IndicatorLED::setSolid(bool on) {
    solidState = on;
    eventBlinking = false;
    digitalWrite(pin, solidState ? HIGH : LOW);
    ledState = solidState;
}

void IndicatorLED::blinkEvent(unsigned long duration) {
    eventBlinking = true;
    eventEndTime = millis() + duration;
    ledState = HIGH;
    digitalWrite(pin, HIGH);
}

void IndicatorLED::update() {
    unsigned long now = millis();
    if (eventBlinking) {
        // Check if the event period has ended
        if (now >= eventEndTime) {
            eventBlinking = false;
            digitalWrite(pin, solidState ? HIGH : LOW);
            ledState = solidState;
        } else {
            // Toggle LED at blink interval
            if (now - lastToggle >= blinkInterval) {
                lastToggle = now;
                ledState = !ledState;
                digitalWrite(pin, ledState ? HIGH : LOW);
            }
        }
    } else {
        // If not blinking, ensure LED matches solidState
        if (ledState != solidState) {
            ledState = solidState;
            digitalWrite(pin, ledState ? HIGH : LOW);
        }
    }
}

// -------------------
// FreeRTOS setup blink task
// -------------------
void IndicatorLED::startSetupBlink(unsigned long interval) {
    blinkInterval = interval;
    // Create FreeRTOS task
    xTaskCreatePinnedToCore(blinkTask, "LED Blink Task", 1024, this, 1, &blinkTaskHandle, 1);
}

void IndicatorLED::stopSetupBlink() {
    if (blinkTaskHandle) {
        vTaskDelete(blinkTaskHandle);
        blinkTaskHandle = nullptr;
        digitalWrite(pin, solidState ? HIGH : LOW);
        ledState = solidState;
    }
}

void IndicatorLED::blinkTask(void* param) {
    IndicatorLED* led = static_cast<IndicatorLED*>(param);
    for (;;) {
        led->ledState = !led->ledState;
        digitalWrite(led->pin, led->ledState ? HIGH : LOW);
        vTaskDelay(led->blinkInterval / portTICK_PERIOD_MS);
    }
}
