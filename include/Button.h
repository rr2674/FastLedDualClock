#pragma once
#include <Arduino.h>

class Button {
public:
    Button(uint8_t pin) : pin(pin) {}

    void begin() {
        pinMode(pin, INPUT_PULLUP);
    }

    bool pressed() {
        bool reading = digitalRead(pin);

        if (reading != lastState) {
            lastDebounce = millis();
        }

        lastState = reading;

        if ((millis() - lastDebounce) > debounceTime && reading == LOW) {
            return true;
        }
        return false;
    }

private:
    uint8_t pin;
    bool lastState = HIGH;
    uint32_t lastDebounce = 0;
    const uint32_t debounceTime = 50;
};
