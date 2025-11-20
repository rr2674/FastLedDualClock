#include "MovingPixelDemo.h"

void MovingPixelDemo::begin(CRGB* leds_, int count) {
    leds = leds_;
    numLeds = count;
    reset();
}

void MovingPixelDemo::reset() {
    pos = 0;
    lastMove = millis();
    Serial.println("MovingPixelDemo reset");
}

void MovingPixelDemo::update() {
    if (millis() - lastMove >= speedMs) {
        lastMove = millis();
        pos = (pos + 1) % numLeds;

        // Fade all LEDs slightly — comet blur effect
        for (int i = 0; i < numLeds; i++) {
            leds[i].nscale8_video(200);  
        }

        // Draw the head
        CRGB baseColor =  colorManager.getColor();
        leds[pos] = baseColor;

        // Draw the tail
        for (int i = 1; i <= tailLength; i++) {
            int idx = (pos - i + numLeds) % numLeds;

            // Exponential fade gives a better comet look
            uint8_t fade = 255 * powf(0.6, i);

            CRGB c = baseColor;
            c.nscale8_video(fade);
            leds[idx] += c;  // add instead of overwrite to keep blur smooth
        }

        FastLED.show();
    }
}
