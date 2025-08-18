#include "DevIQ_DateTime.h"
#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

using namespace DeviceIQ_DateTime;

void Date::SetDate(uint16_t year, uint8_t month, uint8_t day) {
    mYear  = constrain(year, 1, 9999);
    mMonth = constrain(month, 1, 12);

    uint8_t mdays = 31;

    if (mMonth == 2) {
        mdays = LeapYear() ? 29 : 28;
    } else if (mMonth == 4 || mMonth == 6 || mMonth == 9 || mMonth == 11) {
        mdays = 30;
    }

    mDay = constrain(day, 1, mdays);
}

String Date::toString(const String& inFormat) const {
    String format = inFormat.isEmpty() ? mFormat : inFormat;
    String result; result.reserve(format.length() + 8);

    for (uint16_t i = 0; i < format.length(); ++i) {
        char c = format[i];
        if (c == '%' && (i + 1) < format.length()) {
            char f = format[++i];
            switch (f) {
                case 'y': {
                    result += String(mYear + 10000).substring(3);
                } break;
                case 'Y': {
                    result += String(mYear + 10000).substring(1);
                } break;
                case 'm': {
                    result += String(mMonth);
                } break;
                case 'M': {
                    result += String(mMonth + 100).substring(1);
                } break;
                case 'd': {
                    result += String(mDay);
                } break;
                case 'D': {
                    result += String(mDay + 100).substring(1);
                } break;
                default: {
                    result += '%'; result += f;
                } break;
            }
        } else {
            result += c;
        }
    }
    return result;
}

void Time::SetTime(uint8_t hour, uint8_t minute, uint8_t second) {
    mHour = constrain(hour,   0, 23);
    mMinute = constrain(minute, 0, 59);
    mSecond = constrain(second, 0, 59);
}

String Time::toString(const String& inFormat) const {
    String format = inFormat.isEmpty() ? mFormat : inFormat;
    String result; result.reserve(format.length() + 8);

    auto hour12 = [this]() -> uint8_t { uint8_t h = mHour % 12; return (h == 0) ? 12 : h; };

    for (uint16_t i = 0; i < format.length(); ++i) {
        char c = format[i];
        if (c == '%' && (i + 1) < format.length()) {
            char f = format[++i];
            switch (f) {
                case 'h': {
                    result += String(hour12() + 100).substring(1);
                } break;
                case 'H': {
                    result += String(mHour + 100).substring(1);
                } break;
                case 'M': {
                    result += String(mMinute + 100).substring(1);
                } break;
                case 'S': {
                    result += String(mSecond + 100).substring(1);
                } break;
                case 'T': {
                    result += (mHour >= 12 ? "PM" : "AM");
                } break;
                default: {
                    result += '%'; result += f;
                } break;
            }
        } else {
            result += c;
        }
    }
    return result;
}

static inline void tm_from_epoch_with_tz(time_t epochUTC, int32_t tzSeconds, struct tm& out) {
    time_t t = epochUTC + tzSeconds;
#if defined(ESP32) || defined(ESP8266)
    gmtime_r(&t, &out);
#else
    struct tm* p = gmtime(&t);
    if (p) out = *p;
#endif
}

Time Clock::CurrentTime() const {
    struct tm tmv;
    tm_from_epoch_with_tz(mEpochTime, mTimezone * 3600, tmv);
    return Time(tmv.tm_hour, tmv.tm_min, tmv.tm_sec);
}

Date Clock::CurrentDate() const {
    struct tm tmv;
    tm_from_epoch_with_tz(mEpochTime, mTimezone * 3600, tmv);
    return Date(tmv.tm_year + 1900, tmv.tm_mon + 1, tmv.tm_mday);
}

bool Clock::NTPUpdate(const String& ntpserver) {
    if (WiFi.status() != WL_CONNECTED) return false;

    WiFiUDP udpClient;

    NTPClient ntp(udpClient, ntpserver.c_str());
    ntp.setTimeOffset(mTimezone * 3600);
    ntp.begin();

    bool updated = ntp.update();
    if (updated) mEpochTime = ntp.getEpochTime();

    return updated;
}


