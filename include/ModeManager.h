#pragma once

enum Mode {
    MODE_PIXEL,
    MODE_DIGITS
};

class ModeManager {
public:
    ModeManager() : current(MODE_DIGITS) {}

    Mode get() const { return current; }

    bool switchMode() {
        Mode newMode = (current == MODE_PIXEL) ? MODE_DIGITS : MODE_PIXEL;

        if (newMode != current) {
            current = newMode;
            return true;
        }
        return false;
    }

private:
    Mode current;
};
