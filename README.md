# ESP32 LED Clock Display Project  

## Quick start

1. Install Visual Studio Code and the PlatformIO extension.
2. Open this folder in VS Code.
3. Build and upload using PlatformIO (left Activity bar -> PlatformIO -> Project Tasks -> Build / Upload) or use the PlatformIO toolbar.

### *FastLED-based Seven-Segment Digit Renderer + Moving Pixel Demo*
This project demonstrates how to drive a WS2811/WS2812 LED strip using an ESP32, with two selectable display modes:

1. **Moving Pixel Demo** – A simple animated pixel that moves across the strip  
2. **Digit Display Demo** – A seven-segment style numeric display (0–9) rendered across multiple mapped LED segments

A physical **push-button on GPIO 14** switches between modes.  
All animation and mode-switch logic is **non-blocking** (no `delay()` calls).

---

## Features

### Two fully-independent display modes
- **MovingPixelDemo**  
  - Smooth animated pixel using FastLED  
  - Non-blocking `millis()`-based timing  

- **DigitDisplayDemo**  
  - Renders all digits *simultaneously*  
  - Uses a clean data model:
    - `Element` (shape: number of segments × pixels per segment)  
    - `DisplayElement` (location, type, color)  
  - Seven-segment mapping defined once in `DigitDisplayDemo.cpp`  
  - Supports DIGIT, COLON, DASH element types  
  - Works with arbitrarily-sized LED strips using layout validation

### Clean offset-based LED mapping
Each digit or symbol occupies a fixed range:

```cpp
{ "min_ones",     DIGIT,  0,  CRGB::Red   },
{ "min_tens",     DIGIT, 29,  CRGB::Green },
{ "colon_bottom", COLON, 58,  CRGB::Yellow },
{ "colon_top",    COLON, 61,  CRGB::Yellow },
{ "hour_ones",    DIGIT, 63,  CRGB::Blue },
{ "hour_tens",    DIGIT, 92,  CRGB::White }

These offsets allow the LED strip to be treated as a flat linear array while preserving a clear positional layout.

---

## Hardware Requirements
- ESP32 Dev Board
- WS2811 LED strip (need 120 LEDs Pixels)
- Push-button
    - GPIO 14
    - Pulled up to 3.3V (internal pull-up enabled)

## Wiring
- Data pin: change `DATA_PIN` in `src/main.cpp` to match your LED strip data pin.
- Ground: connect ESP32 GND to LED strip GND.
- Power: provide appropriate 12V power to LED strip.


License: MIT
