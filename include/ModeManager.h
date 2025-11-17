#pragma once

enum Mode {
    MODE_PIXEL,
    MODE_DIGITS
};

class ModeManager {
public:
    ModeManager() : current(MODE_DIGITS) {}

    Mode getMode() const { return current; }

    void switchMode() {

        current = (current == MODE_PIXEL) ? MODE_DIGITS : MODE_PIXEL;
        Serial.printf("Switched mode to: %s\n",
                      (current == MODE_PIXEL) ? "PIXEL" : "DIGITS");
    }

private:
    Mode current;
};
