#pragma once
#include <Arduino.h>

class AppManager {
public:

    enum class AppName : uint8_t {
        PIXEL,
        DIGITS,
        DUALCLOCK,
        COUNT_      // always keep this last
    };

    AppManager() : current(AppName::DUALCLOCK) {}

    AppName getApp() const { return current; }

    void switchApp() {
        current = static_cast<AppName>(
            (static_cast<uint8_t>(current) + 1) %
            static_cast<uint8_t>(AppName::COUNT_)
        );
        
#ifdef DEBUG_MODE
        Serial.printf("Switched App to: %s\n", appNameToString(current));
#endif
    }

private:
    AppName current;

    const char* appNameToString(AppName m) const {
        switch (m) {
            case AppName::PIXEL:     return "PIXEL";
            case AppName::DIGITS:    return "DIGITS";
            case AppName::DUALCLOCK: return "DUALCLOCK";
            default:                 return "UNKNOWN";
        }
    }
};
