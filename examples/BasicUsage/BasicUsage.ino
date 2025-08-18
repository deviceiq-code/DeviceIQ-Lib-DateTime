#include <WiFi.h>
#include "DevIQ_DateTime.h"

using namespace DeviceIQ_DateTime;

const char* ssid     = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

Clock clock;

void setup() {
    Serial.begin(115200);
    Serial.println();
    Serial.println("Connecting to WiFi...");

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.println("WiFi connected");

    if (clock.NTPUpdate("pool.ntp.org")) {
        Serial.println("Time updated from NTP successfully!");
    } else {
        Serial.println("Failed to update time from NTP");
    }
}

void loop() {
    // Keep epoch time updated internally
    clock.Control();

    // Print formatted date and time
    Serial.print("Date: ");
    Serial.println(clock.CurrentDate().toString("%Y-%M-%D"));

    Serial.print("Time: ");
    Serial.println(clock.CurrentTime().toString("%H:%M:%S %T"));

    delay(1000);
}
