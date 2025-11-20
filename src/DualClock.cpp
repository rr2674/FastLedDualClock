#include "DualClock.h"
#include "DisplayModel.h"
#include "ColorManager.h"

DualClock::DualClock(const char* ssid, const char* password)
    : wifiSSID(ssid),
      wifiPassword(password)
{
#ifdef DEBUG_MODE
    debug = true;
    Serial.println("[DEBUG] DualClock initialized in DEBUG mode");
#endif

    reset();
}

void DualClock::begin(CRGB* leds_, int numLeds_) {
    leds = leds_;

    if (!DisplayModel::validateLayout(numLeds_)) {
        Serial.println("ERROR: LED strip is too short for DualClock!");
    }

    WiFi.begin(wifiSSID, wifiPassword);
    if (debug) {
        Serial.print("[DEBUG] Connecting to WiFi");
    }
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        if (debug) {
            Serial.print(".");
        }
    }
    Serial.println("\nWiFi connected, IP: " + WiFi.localIP().toString());

    while ( !syncTimeHTTP() ) {
        delay(2*1000); // wait 2 seconds before each retry
        Serial.println("Failed to sync time via HTTP. Retry...");
    }

    Serial.println("Setup complete!");
}

void DualClock::update() {

    if (millis() - lastUpdate >= 1 * 1000) {
        lastUpdate = millis();

        switch(modeManager.get()) {
            case DualClockModeManager::Mode::TIME:
                displayTime();
                break;

            case DualClockModeManager::Mode::DATE:
                displayDate();
                break;
        }
    }
}

void DualClock::switchMode() {
    modeManager.next();

    // Clear all LEDs; account for mapping differences between colon and dahs
    FastLED.clear();
    FastLED.show();

}

void DualClock::reset() {
    modeManager.reset();
    Serial.println("DualClock reset");
}

int DualClock::getHour(bool as24hr) const {
    time_t now = time(nullptr);
    struct tm* local = localtime(&now);
    int hour = local->tm_hour;

    if (as24hr) {
        return hour;
    }

    return (hour % 12 == 0) ? 12 : (hour % 12);
}

void DualClock::switchHourFormat() {
    use24Hour = !use24Hour;
}

void DualClock::setHourFormat(bool use24hr) {
    use24Hour = use24hr;
}

/*
** private methods
*/

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
    tz.setLocation(F("America/Chicago"));
    tz.setDefault();

    Serial.printf("Time synced: %02d:%02d:%02d UTC\n", hour, minute, second);
    return true;
}

void DualClock::displayTime() {
    time_t now = time(nullptr);
    struct tm* tinfo = localtime(&now);

    int hourVal = tinfo->tm_hour;
    int minVal  = tinfo->tm_min;
    int secVal  = tinfo->tm_sec;

    if (!use24Hour) {
        // Convert to 12-hour format, hanlde midnight
        hourVal = (hourVal % 12 == 0) ? 12 : (hourVal % 12);
    }

    for (size_t i = 0; i < DisplayModel::getTimeDisplayCount(); ++i) {
        const auto& el = DisplayModel::getTimeDisplay()[i];

        switch (el.type) {
            case DisplayElementType::DIGIT: {
                int digit = DisplayModel::computeDigit(el.role, minVal, hourVal);
                renderDigitElement(el, digit);
                break;
            }
            case DisplayElementType::COLON: {
                renderColonOrDash(el, secVal % 2 == 0);
                break;
            }
            default: {
                Serial.println("DualClock::displayTime() unexpected el.type");
                break;
            }            
        }
    }

    FastLED.show();

    if (debug) {
        Serial.printf("[DEBUG] time[hh:mm] %02d:%02d\n", hourVal, minVal);
    }

}

void DualClock::displayDate() {
    time_t now = time(nullptr);
    struct tm* tinfo = localtime(&now);

    int dayVal   = tinfo->tm_mday;
    int monthVal = tinfo->tm_mon + 1; // tm_mon is 0-based
    int secVal   = tinfo->tm_sec;

    for (size_t i = 0; i < DisplayModel::getDateDisplayCount(); ++i) {
        const auto& el = DisplayModel::getDateDisplay()[i];

        switch (el.type) {
            case DisplayElementType::DIGIT: {
                int digit = DisplayModel::computeDigit(el.role, dayVal, monthVal);
                renderDigitElement(el, digit);
                break;
            }

            case DisplayElementType::DASH: {
                renderColonOrDash(el, secVal % 2 == 0); 
                break;
            }
            default: {
                Serial.println("DualClock::displayDate() unexpected el.type");
                break;
            }  
        }
    }

    FastLED.show();

    if (debug) {
        Serial.printf("[DEBUG] Date[mm-dd] %02d-%02d\n", dayVal, monthVal);
    }
}

void DualClock::renderDigitElement(const DisplayElement& el, int number) {
    const auto& shape = DisplayModel::getElementShape(el.type);
    const auto& map = DisplayModel::getDigitSegmentMap();

    for (uint8_t seg = 0; seg < shape.segments; ++seg) {
        bool isOn = map[number][seg];
        CRGB segColor = isOn ? colorManager.getColor() : CRGB::Black;

        int segmentStart = el.offset + (seg * shape.pixels);
        fill_solid(&leds[segmentStart], shape.pixels, segColor);
    }

    if (debug) {
        Serial.printf("[DEBUG] Rendered %s digit %d at offset %d\n", el.name, number, el.offset);
    }
}

void DualClock::renderColonOrDash(const DisplayElement& el, bool on) {
    const auto& shape = DisplayModel::getElementShape(el.type);

    CRGB c = on ? colorManager.getColor() : CRGB::Black;

    for (uint8_t seg = 0; seg < shape.segments; ++seg) {
        int segmentStart = el.offset + (seg * shape.pixels);
        fill_solid(&leds[segmentStart], shape.pixels, c);
    }

    if (debug) {
        Serial.printf("[DEBUG] Rendered %s at offset %d\n", el.name, el.offset);
    }
}
