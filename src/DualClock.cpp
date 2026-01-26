#include "DualClock.h"
#include "DisplayModel.h"
#include "ColorManager.h"

DualClock::DualClock(const char *ssid, const char *password, const char *timezone)
    : wifiSSID(ssid),
      wifiPassword(password),
      tzName(timezone)
{
#ifdef DEBUG_MODE
    debug = true;
    Serial.println("[DEBUG] DualClock initialized in DEBUG mode");
#endif

    reset();
}

void DualClock::begin(CRGB *leds_, int numLeds_)
{
    leds = leds_;

    if (!DisplayModel::validateLayout(numLeds_))
    {
        Serial.println("ERROR: LED strip is too short for DualClock!");
    }

    WiFi.begin(wifiSSID, wifiPassword);
    if (debug)
    {
        Serial.print("[DEBUG] Connecting to WiFi");
    }
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        if (debug)
        {
            Serial.print(".");
        }
    }
    Serial.println("\nWiFi connected, IP: " + WiFi.localIP().toString());
    Serial.println("Wifi MAC: " + WiFi.macAddress());

    while (!syncTimeHTTP())
    {
        delay(2 * 1000); // wait 2 seconds before each retry
        Serial.println("Failed to sync time via HTTP. Retry...");
    }

    if (debug)
    {
        Serial.println("[DEBUG] Setup complete!");
    }
}

void DualClock::update()
{

    if (millis() - lastUpdate >= 1 * 1000)
    {
        lastUpdate = millis();

        // Periodic HTTP sync
        if (lastUpdate - lastHttpSync >= httpSyncInterval)
        {
            if (syncTimeHTTP())
            {
                lastHttpSync = lastUpdate;
            }
            else
            {
                Serial.println("[WARNING] HTTP time sync FAILED.");
            }
        }

        switch (modeManager.get())
        {
        case DualClockModeManager::Mode::TIME:
            displayTime();
            break;

        case DualClockModeManager::Mode::DATE:
            displayDate();
            break;
        }
    }
}

void DualClock::switchMode()
{
    modeManager.next();

    // Clear all LEDs; account for mapping differences between colon and dash
    FastLED.clear();
    FastLED.show();
}

void DualClock::reset()
{
    modeManager.reset();
    if (debug)
    {
        Serial.println("[DEBUG] DualClock reset");
    }
}

int DualClock::getHour(bool as24hr) const
{
    time_t now = time(nullptr);
    struct tm *local = localtime(&now);
    int hour = local->tm_hour;

    if (as24hr)
    {
        return hour;
    }

    return (hour % 12 == 0) ? 12 : (hour % 12);
}

void DualClock::switchHourFormat()
{
    use24Hour = !use24Hour;
}

void DualClock::setHourFormat(bool use24hr)
{
    use24Hour = use24hr;
}

/*
** private methods
*/

bool DualClock::syncTimeHTTP()
{

    if (WiFi.status() != WL_CONNECTED)
        return false;

    HTTPClient http;
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

    http.begin("http://worldtimeapi.org/api/ip");

    const char *headers[] = {
        "Content-Type",
        "Content-Length",
        "Location",
        "Server",
        "Date",
        "Connection"};

    http.collectHeaders(headers, sizeof(headers) / sizeof(headers[0]));

    String contentType = http.header("Content-Type");

    int code = http.GET();

    if (code != 200)
    {
        Serial.println("HTTP GET failed");
        http.end();
        return false;
    }

    String payload = http.getString();

    Serial.printf("Payload length: %d\n", payload.length());
    Serial.println("Payload preview:");
    Serial.println(payload.substring(0, 200));

    for (size_t i = 0; i < http.headers(); i++)
    {
        Serial.printf("Header[%d]: %s = %s\n",
                      i,
                      http.headerName(i).c_str(),
                      http.header(i).c_str());
    }

    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, payload);
    http.end();

    if (error)
    {
        Serial.print("JSON parse error: ");
        Serial.println(error.c_str());
        Serial.printf("HTTP code: %d\n", code);
        Serial.printf("Content-Type: %s\n", contentType.c_str());
        return false;
    }

    const char *datetime = doc["datetime"];
    // e.g., "2025-11-03T15:43:02.123456+00:00"
    struct tm t = {};
    t.tm_year = atoi(datetime + 0) - 1900;
    t.tm_mon = atoi(datetime + 5) - 1;
    t.tm_mday = atoi(datetime + 8);
    t.tm_hour = atoi(datetime + 11);
    t.tm_min = atoi(datetime + 14);
    t.tm_sec = atoi(datetime + 17);

    time_t now = mktime(&t);
    struct timeval tv = {now, 0};
    settimeofday(&tv, nullptr);

    // Sync ezTime with system time
    UTC.setTime(now);
    tz.setLocation(tzName);
    tz.setDefault();

    if (debug)
    {
        Serial.printf("[DEBUG] Time synced: %02d:%02d:%02d UTC\n", hour, minute, second);
    }

    return true;
}

void DualClock::checkWiFi()
{

    if (WiFi.status() == WL_CONNECTED)
    {
        wifiDownStart = 0; // reset downtime
        return;
    }

    // Start the downtime clock
    if (wifiDownStart == 0)
    {
        wifiDownStart = millis();
        if (debug)
        {
            Serial.println("[DEBUG] DualClock::checkWifi() Disconnected detected");
        }
    }

    unsigned long now = millis();

    // Reboot if WiFi has been down for > 30 minutes
    const unsigned long THIRTY_MINUTES = 30UL * 60UL * 1000UL;
    if (now - wifiDownStart > THIRTY_MINUTES)
    {
        FastLED.clear();
        FastLED.show();
        ESP.restart();
    }

    // Attempt reconnect every 5 seconds
    if (now - lastWifiReconnectAttempt > 5 * 1000)
    {
        lastWifiReconnectAttempt = now;
        if (debug)
        {
            Serial.println("[DEBUG] DualClock::checkWifi() Attempting reconnect...");
        }
        WiFi.reconnect();
    }
}

void DualClock::displayTime()
{
    time_t now = time(nullptr);
    struct tm *tinfo = localtime(&now);

    int hourVal = tinfo->tm_hour;
    int minVal = tinfo->tm_min;
    int secVal = tinfo->tm_sec;

    if (!use24Hour)
    {
        // Convert to 12-hour format, hanlde midnight
        hourVal = (hourVal % 12 == 0) ? 12 : (hourVal % 12);
    }

    for (size_t i = 0; i < DisplayModel::getTimeDisplayCount(); ++i)
    {
        const auto &el = DisplayModel::getTimeDisplay()[i];

        switch (el.type)
        {
        case DisplayModel::DisplayElementType::DIGIT:
        {
            int digit = DisplayModel::computeDigit(el.role, minVal, hourVal);
            renderDigitElement(el, digit);
            break;
        }
        case DisplayModel::DisplayElementType::COLON:
        {
            renderColonOrDash(el, secVal % 2 == 0);
            break;
        }
        default:
        {
            Serial.println("DualClock::displayTime() unexpected el.type");
            break;
        }
        }
    }

    FastLED.show();

    if (debug)
    {
        Serial.printf("[DEBUG] time[hh:mm] %02d:%02d\n", hourVal, minVal);
    }
}

void DualClock::displayDate()
{
    time_t now = time(nullptr);
    struct tm *tinfo = localtime(&now);

    int dayVal = tinfo->tm_mday;
    int monthVal = tinfo->tm_mon + 1; // tm_mon is 0-based
    int secVal = tinfo->tm_sec;

    for (size_t i = 0; i < DisplayModel::getDateDisplayCount(); ++i)
    {
        const auto &el = DisplayModel::getDateDisplay()[i];

        switch (el.type)
        {
        case DisplayModel::DisplayElementType::DIGIT:
        {
            int digit = DisplayModel::computeDigit(el.role, dayVal, monthVal);
            renderDigitElement(el, digit);
            break;
        }

        case DisplayModel::DisplayElementType::DASH:
        {
            renderColonOrDash(el, secVal % 2 == 0);
            break;
        }
        default:
        {
            Serial.println("DualClock::displayDate() unexpected el.type");
            break;
        }
        }
    }

    FastLED.show();

    if (debug)
    {
        Serial.printf("[DEBUG] Date[mm-dd] %02d-%02d\n", dayVal, monthVal);
    }
}

void DualClock::renderDigitElement(const DisplayModel::DisplayElement &el, int number)
{
    const auto &shape = DisplayModel::getElementShape(el.type);
    const auto &map = DisplayModel::getDigitSegmentMap();

    // Turn off tens digit if 12-hour mode and tens is 0
    bool blackoutLeadingZero = false;
    if (!use24Hour && el.role == DisplayModel::DigitRole::TENS_2 && number == 0)
    {
        blackoutLeadingZero = true;
    }

    for (uint8_t seg = 0; seg < shape.segments; ++seg)
    {
        bool isOn = map[number][seg];
        CRGB segColor = isOn ? colorManager.getColor() : CRGB::Black;

        // override color assigment
        if (blackoutLeadingZero)
        {
            segColor = CRGB::Black;
        }

        int segmentStart = el.offset + (seg * shape.pixels);
        fill_solid(&leds[segmentStart], shape.pixels, segColor);
    }

    if (debug)
    {
        Serial.printf("[DEBUG] Rendered %s digit %d at offset %d\n", el.name, number, el.offset);
    }
}

void DualClock::renderColonOrDash(const DisplayModel::DisplayElement &el, bool on)
{
    const auto &shape = DisplayModel::getElementShape(el.type);

    CRGB c = on ? colorManager.getColor() : CRGB::Black;

    for (uint8_t seg = 0; seg < shape.segments; ++seg)
    {
        int segmentStart = el.offset + (seg * shape.pixels);
        fill_solid(&leds[segmentStart], shape.pixels, c);
    }

    if (debug)
    {
        Serial.printf("[DEBUG] Rendered %s at offset %d\n", el.name, el.offset);
    }
}
