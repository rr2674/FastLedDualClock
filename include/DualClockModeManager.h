#pragma once
#include <Arduino.h>

class DualClockModeManager {
public:
    enum class Mode : uint8_t {
        TIME,
        DATE,
        MODE_COUNT  // Keep last
    };

    DualClockModeManager() : current(Mode::TIME) {}

    Mode get() const { return current; }

    void next() {
        uint8_t v = static_cast<uint8_t>(current);
        v = (v + 1) % static_cast<uint8_t>(Mode::MODE_COUNT);
        current = static_cast<Mode>(v);
    }

    void reset() {
        current = Mode::TIME;
    }

    const char* toString() const {
        switch (current) {
            case Mode::TIME: return "TIME";
            case Mode::DATE: return "DATE";
            default:         return "UNKNOWN";
        }
    }

private:
    Mode current;
};
