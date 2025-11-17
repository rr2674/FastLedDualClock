# FastLedDualClock


Quick start

1. Install Visual Studio Code and the PlatformIO extension (recommended in `.vscode/extensions.json`).
2. Open this folder in VS Code.
3. Build and upload using PlatformIO (left Activity bar -> PlatformIO -> Project Tasks -> Build / Upload) or use the PlatformIO toolbar.

Wiring (example)
- Data pin: change `DATA_PIN` in `src/main.cpp` to match your LED strip data pin.
- Ground: connect ESP32 GND to LED strip GND.
- Power: provide appropriate 5V (or 12V) power to LED strip; do NOT power many LEDs from the ESP32 5V pin directly.


License: MIT
