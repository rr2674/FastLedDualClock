#pragma once
#include <Arduino.h>

//todo: make this array as well, and name space the variables?
enum Mode : uint8_t {
    MODE_PIXEL,
    MODE_DIGITS,
    MODE_DUALCLOCK,
    MODE_COUNT      // always keep this last
};

class ModeManager {
public:
    ModeManager() : current(MODE_DIGITS) {}

    Mode getMode() const { return current; }

    void switchMode() {
        // Cycle through the enum values
        current = static_cast<Mode>((static_cast<uint8_t>(current) + 1) % MODE_COUNT);
        
        Serial.printf("Switched mode to: %s\n", modeToString(current));
    }

private:
    Mode current;

    const char* modeToString(Mode m) const {
        switch (m) {
            case MODE_PIXEL:     return "PIXEL";
            case MODE_DIGITS:    return "DIGITS";
            case MODE_DUALCLOCK: return "DUALCLOCK";
            default:             return "UNKNOWN";
        }
    }
};
