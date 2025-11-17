#pragma once
#include <Arduino.h>

class Button {
public:
    Button(uint8_t pin) : pin(pin) {
        pinMode(pin, INPUT_PULLDOWN);
        
        // Capture initial state to prevent a false press at boot
        lastStableState = digitalRead(pin);
        lastRawState    = lastStableState;
        lastChangeTime  = millis();
    }

    bool pressed() {
        bool reading = digitalRead(pin);

        if (reading != lastReading) {
            lastDebounceTime = millis();
        }

        lastReading = reading;

        if ((millis() - lastDebounceTime) > debounceDelay) {
            if (reading == LOW && !alreadyPressed) {
                alreadyPressed = true;
                return true;
            } else if (reading == HIGH) {
                alreadyPressed = false;  // reset for next press
            }
        }
        return false;
    }

private:
    uint8_t pin;
    bool lastReading = HIGH;
    bool alreadyPressed = false;
    unsigned long lastDebounceTime = 0;
    const unsigned long debounceDelay = 50; // milliseconds
};
