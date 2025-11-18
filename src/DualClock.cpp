#include "DualClock.h"
#include "DisplayModel.h"
#include "ColorManager.h"

DualClock::DualClock(const char* ssid, const char* password)
    : wifiSSID(ssid),
     wifiPassword(password)
{
#ifdef DEBUG_MODE
    this->debug = true;
    Serial.println("DualClock initialized in DEBUG mode");
#endif


    this->reset();
}

void DualClock::begin(CRGB* leds_, int numLeds_) {
    this->leds = leds_;

    if (!validateLayout(numLeds_)) {
        Serial.println("Warning: LED strip is too short for DualClock!");
    }

    WiFi.begin(this->wifiSSID, this->wifiPassword);
    if (this->debug) {
        Serial.print("Connecting to WiFi");
    }
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        if (this->debug) {
            Serial.print(".");
        }
    }
    Serial.println("\nWiFi connected, IP: " + WiFi.localIP().toString());

    while ( !this->syncTimeHTTP() ) {
        delay(2*1000); // wait 2 seconds before each retry
        Serial.println("Failed to sync time via HTTP. Retry...");
    }

    Serial.println("Setup complete!");
}

void DualClock::update() {

    if (millis() - this->lastUpdate >= 1 * 1000) {
        this->lastUpdate = millis();

        switch(this->currentMode) {
            case DisplayMode::TIME:
                this->displayTime();
                break;

            case DisplayMode::DATE:
                this->displayDate();
                break;
        }
    }
}

void DualClock::reset() {
    this->currentMode = DisplayMode::TIME;
    Serial.println("DualClock reset");
}

void DualClock::switchMode() {
    this->currentMode = static_cast<DisplayMode>((static_cast<int>(this->currentMode) + 1) % static_cast<int>(DisplayMode::DISPLAY_MODE_COUNT));
    Serial.printf("DualClock() Switched mode to: %s\n", modeToString(this->currentMode));
}

/*
** private methods
*/

bool DualClock::validateLayout(int numLeds) {
    int requiredPixels = 0;

    for (int i = 0; i < NUM_TIME_DISPLAY_ELEMENTS; i++) {
        const DisplayElement& el = timeDisplay[i];
        const Element& shape = getElementShape(el.type);

        int elementPixels = el.offset + (shape.segments * shape.pixels);

        if (elementPixels > requiredPixels) {
            requiredPixels = elementPixels;
        }
    }

    if (requiredPixels > numLeds) {
        Serial.printf("DualClock layout requires %d LEDs, but only %d available\n",
                      requiredPixels, numLeds);
        return false;
    }

    return true;
}

bool DualClock::syncTimeHTTP() {
    if (WiFi.status() != WL_CONNECTED) return false;

    HTTPClient http;
    http.begin("http://worldtimeapi.org/api/ip");
    int code = http.GET();

    if (code != 200) {
        Serial.println("HTTP GET failed");
        http.end();
        return false;
    }

    String payload = http.getString();
    http.end();

    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, payload);
    if (error) {
        Serial.println("JSON parse error!");
        return false;
    }

    const char* datetime = doc["datetime"]; // e.g., "2025-11-03T15:43:02.123456+00:00"
    int year   = atoi(String(datetime).substring(0,4).c_str());
    int month  = atoi(String(datetime).substring(5,7).c_str());
    int day    = atoi(String(datetime).substring(8,10).c_str());
    int hour   = atoi(String(datetime).substring(11,13).c_str());
    int minute = atoi(String(datetime).substring(14,16).c_str());
    int second = atoi(String(datetime).substring(17,19).c_str());

    struct tm t;
    t.tm_year = year - 1900;
    t.tm_mon  = month - 1;
    t.tm_mday = day;
    t.tm_hour = hour;
    t.tm_min  = minute;
    t.tm_sec  = second;
    time_t now = mktime(&t);

    struct timeval tv = { now, 0 };
    settimeofday(&tv, nullptr);

    // Sync ezTime with system time
    UTC.setTime(now);
    this->tz.setLocation(F("America/Chicago"));
    this->tz.setDefault();

    Serial.printf("Time synced: %02d:%02d:%02d UTC\n", hour, minute, second);
    return true;
}

void DualClock::displayTime() {
    time_t now = time(nullptr);
    struct tm* tinfo = localtime(&now);

    int hourVal = tinfo->tm_hour;
    int minVal  = tinfo->tm_min;
    int secVal  = tinfo->tm_sec;

    // min_ones
    renderDigitElement(timeDisplay[0], minVal % 10);
    // min_tens
    renderDigitElement(timeDisplay[1], minVal / 10);
    // hour_ones
    renderDigitElement(timeDisplay[4], hourVal % 10);
    // hour_tens
    renderDigitElement(timeDisplay[5], hourVal / 10);

    // Blink colon
    renderColonOrDash(timeDisplay[2], secVal % 2 == 0);
    renderColonOrDash(timeDisplay[3], secVal % 2 == 0);

    FastLED.show();

    if (this->debug) {
        Serial.printf("[DEBUG] time[hh:mm] %02d:%02d\n", hourVal, minVal);
    }

}

void DualClock::displayDate() {
    time_t now = time(nullptr);
    struct tm* tinfo = localtime(&now);

    int dayVal   = tinfo->tm_mday;
    int monthVal = tinfo->tm_mon + 1;

    // day_ones
    renderDigitElement(dateDisplay[0], dayVal % 10);
    // day_tens
    renderDigitElement(dateDisplay[1], dayVal / 10);
    // month_ones
    renderDigitElement(dateDisplay[4], monthVal % 10);
    // month_tens
    renderDigitElement(dateDisplay[5], monthVal / 10);


    FastLED.show();

    if (this->debug) {
        Serial.printf("[DEBUG] Date[mm-dd] %02d-%02d\n", dayVal, monthVal);
    }
}

void DualClock::renderDigitElement(const DisplayElement& el, int number) {
    const Element& shape = getElementShape(el.type);

    for (int seg = 0; seg < shape.segments; seg++) {
        bool isOn = digitSegmentMap[number][seg];
        CRGB segColor = isOn ? this->colorManager.getColor() : CRGB::Black;

        int segmentStart = el.offset + (seg * shape.pixels);
        fill_solid(&leds[segmentStart], shape.pixels, segColor);
    }

    if (this->debug) {
        Serial.printf("[DEBUG] Rendered %s digit %d at offset %d\n", el.name, number, el.offset);
    }
}

void DualClock::renderColonOrDash(const DisplayElement& el, bool on) {
    const Element& shape = getElementShape(el.type);

    CRGB c = on ? this->colorManager.getColor() : CRGB::Black;

    for (int seg = 0; seg < shape.segments; seg++) {
        int segmentStart = el.offset + (seg * shape.pixels);
        fill_solid(&leds[segmentStart], shape.pixels, c);
    }

    if (this->debug) {
        Serial.printf("[DEBUG] Rendered %s at offset %d\n", el.name, el.offset);
    }
}
