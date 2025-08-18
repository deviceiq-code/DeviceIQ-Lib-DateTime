# DeviceIQ Lib DateTime
`DeviceIQ Lib DateTime` is a lightweight date, time, and clock utility library for Arduino/ESP32 projects. It provides easy manipulation, formatting, and NTP synchronization without holding persistent NTP client objects in memory.

Features
--------
- Date
  - Stores day, month, and year
  - Calculates weekday (1 = Monday, 7 = Sunday)
  - Leap year detection
  - Custom formatting with tokens (e.g. %D/%M/%Y)

- Time
  - Stores hour, minute, and second
  - 12-hour or 24-hour formats
  - Custom formatting with tokens (e.g. %H:%M:%S, %h:%M %T)

- Timer
  - Simple periodic timer with millisecond resolution
  - Callback-based execution on timeout
  - Start, stop, reset methods

- Clock
  - Maintains current epoch time in UTC
  - Tracks time progression using an internal timer
  - Supports timezone offsets (−12 to +14 hours)
  - Stateless NTP synchronization — creates an NTPClient only when needed to update the clock, then destroys it immediately

Installation
------------
1. Copy DevIQ_DateTime.h and DevIQ_DateTime.cpp into your Arduino project.
2. Install dependencies:
   - NTPClient library (https://github.com/arduino-libraries/NTPClient)
   - ESP32/ESP8266 WiFi libraries (WiFi.h, WiFiUdp.h)

Usage Example
-------------

```cpp
#include "DevIQ_DateTime.h"
#include <WiFi.h>

using namespace DeviceIQ_DateTime;

Clock clock;

void setup() {
    Serial.begin(115200);
    WiFi.begin("SSID", "PASSWORD");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    if (clock.NTPUpdate("pool.ntp.org")) {
        Serial.println("Time updated from NTP");
    } else {
        Serial.println("Failed to update time from NTP");
    }
}

void loop() {
    clock.Control(); // keep the internal epoch timer updated

    Serial.println(clock.CurrentDate().toString("%Y-%M-%D"));
    Serial.println(clock.CurrentTime().toString("%H:%M:%S"));
    delay(1000);
}
```

Date Formatting Tokens
----------------------
%Y  Year (4 digits)               -> 2025
%y  Year (last 2 digits)          -> 25
%M  Month (2 digits)              -> 03
%m  Month (no leading zero)       -> 3
%D  Day (2 digits)                -> 09
%d  Day (no leading zero)         -> 9

Time Formatting Tokens
----------------------
%H  Hour (00–23)                  -> 14
%h  Hour (01–12)                  -> 02
%M  Minute (00–59)                -> 05
%S  Second (00–59)                -> 09
%T  AM/PM                         -> PM

Timezone Reference
------------------
The Clock class accepts timezones from -12 to +14 hours.

| Offset | Example Locations            |
|--------|------------------------------|
| -12    | Baker Island, Howland Island |
| -11    | American Samoa, Niue         |
| -10    | Hawaii, Tahiti               |
| -9     | Alaska                       |
| -8     | Los Angeles, Vancouver       |
| -7     | Denver, Phoenix              |
| -6     | Chicago, Mexico City         |
| -5     | New York, Toronto, Bogota    |
| -4     | Santiago, Caracas            |
| -3     | São Paulo, Buenos Aires      |
| -2     | South Georgia/Sandwich Is.   |
| -1     | Azores                       |
| 0      | London, Lisbon, UTC          |
| +1     | Berlin, Paris, Rome          |
| +2     | Athens, Cairo, Johannesburg  |
| +3     | Moscow, Riyadh, Nairobi      |
| +4     | Dubai, Baku                  |
| +5     | Karachi, Tashkent            |
| +6     | Almaty, Dhaka                |
| +7     | Bangkok, Jakarta             |
| +8     | Beijing, Singapore, Perth    |
| +9     | Tokyo, Seoul                 |
| +10    | Sydney, Vladivostok          |
| +11    | Solomon Islands, Nouméa      |
| +12    | Auckland, Suva               |
| +13    | Tonga, Apia                  |
| +14    | Kiritimati Island            |

Notes
-----
- The `Timezone()` setter in Clock accepts integer offsets only.
- DST (Daylight Saving Time) adjustments must be handled manually in your code.
- Stateless NTP: NTPUpdate() creates a temporary NTPClient instance, fetches the time from the given server, updates the internal epoch, and then destroys the client.
- Timekeeping after synchronization is handled by the Clock class using millis().
- Call Clock::Control() frequently (e.g., in loop()) to maintain accuracy.

License
-------
This library is released under the MIT License.
