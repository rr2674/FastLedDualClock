#pragma once
#include <Arduino.h>

class Button {
public:
    Button(uint8_t pin) : pin(pin) {
        pinMode(pin, INPUT_PULLDOWN);

        // Initialize states
        lastReading     = digitalRead(pin);
        lastStableState = lastReading;
        lastRawState    = lastReading;
        lastChangeTime  = millis();
    }

    // Returns true ONCE per physical button press
    bool pressed() {
        bool reading = digitalRead(pin);

        if (reading != lastRawState) {
            lastChangeTime = millis();    // raw change detected
            lastRawState = reading;
        }

        // Has the state been stable long enough?
        if ((millis() - lastChangeTime) > debounceDelay) {

            if (lastStableState != reading) {
                lastStableState = reading;

                // Detect rising edge (LOW → HIGH)
                if (reading == HIGH) {
                    return true;
                }
            }
        }

        return false;
    }

private:
    uint8_t pin;

    bool lastReading     = LOW;  // previous digitalRead
    bool lastRawState    = LOW;  // raw, noisy reading
    bool lastStableState = LOW;  // debounced stable value

    unsigned long lastChangeTime  = 0;
    const unsigned long debounceDelay = 50;
};
