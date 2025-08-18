// DevIQ_DateTime.h (refatorado)
#ifndef DevIQ_DateTime_h
#define DevIQ_DateTime_h

#pragma once

#include <Arduino.h>
#include <time.h>

namespace DeviceIQ_DateTime {
    using callback_t = std::function<void()>;
    enum TimerStates { TIMERSTOPPED, TIMERRUNNING };

    class Date {
        private:
            uint8_t mDay   = 1;
            uint8_t mMonth = 1;
            uint16_t mYear  = 1;
            String mFormat = "%D/%M/%Y";
        public:
            Date() {}
            Date(uint16_t year, uint8_t month, uint8_t day = 1) { SetDate(year, month, day); }

            String Format() const { return mFormat; }
            void Format(const String& value) { mFormat = value; }
            
            uint8_t Day() const { return mDay; }
            uint8_t Month() const { return mMonth; }
            uint16_t Year()  const { return mYear; }

            uint8_t Weekday() const { uint8_t day = mDay, month = mMonth; uint16_t year = mYear; if (month < 3) { month += 12; year -= 1; } uint8_t h = (day + ((13 * (month + 1)) / 5) + year + (year / 4) - (year / 100) + (year / 400)) % 7; const uint8_t daysOfWeek[] = {7, 1, 2, 3, 4, 5, 6}; return daysOfWeek[h]; }
            bool LeapYear() const { return (mYear % 4 == 0) && ((mYear % 100 != 0) || (mYear % 400 == 0)); }

            void SetDate(uint16_t year, uint8_t month, uint8_t day = 1);
            String toString(const String& format = "") const;
    };

    class Time {
        private:
            uint8_t mSecond = 0;
            uint8_t mMinute = 0;
            uint8_t mHour = 0;
            String  mFormat = "%H:%M:%S";
        public:
            Time() {}
            Time(uint8_t hour, uint8_t minute, uint8_t second = 0) { SetTime(hour, minute, second); }

            String Format() const { return mFormat; }
            void Format(const String& value) { mFormat = value; }

            uint8_t Second() const { return mSecond; }
            uint8_t Minute() const { return mMinute; }
            uint8_t Hour() const { return mHour; }

            void SetTime(uint8_t hour, uint8_t minute, uint8_t second = 0);
            String toString(const String& format = "") const;
    };

    class Timer {
        private:
            callback_t mOnTimeout;
            uint32_t mTimer = 0;
            uint32_t mTimeout = 0;
            TimerStates mState  = TIMERSTOPPED;
        public:
            Timer() {}
            explicit Timer(uint32_t timeout) : mTimeout(timeout) {}

            void Start() { if (mState == TIMERSTOPPED) { mState = TIMERRUNNING; mTimer = millis(); } }
            void Reset() { if (mState == TIMERRUNNING) { mTimer = millis(); } }
            void Stop()  { mState = TIMERSTOPPED; mTimer = 0; }
            uint32_t CurrentTimerMs() const { return (mState == TIMERRUNNING ? millis() - mTimer : 0); }
            void SetTimeout(uint32_t value) { mTimeout = value; }
            TimerStates State() const { return mState; }
            void Control() { if (mState == TIMERRUNNING && mTimeout > 0 && CurrentTimerMs() >= mTimeout) { Reset(); if (mOnTimeout) mOnTimeout(); } }
            void OnTimeout(callback_t callback) { mOnTimeout = callback; }
    };

    class Clock {
        private:
            Timer mEpochUpdater;
            time_t mEpochTime = 0;
            int8_t mTimezone  = -3;
        public:
            Clock() : mEpochUpdater(1000) { mEpochUpdater.OnTimeout([this]{ mEpochTime++; }); mEpochUpdater.Start(); }

            Time CurrentTime() const;
            Date CurrentDate() const;

            int8_t Timezone() const { return mTimezone; }
            void Timezone(int8_t tz) { mTimezone = constrain(tz, -12, 14); }

            bool NTPUpdate(const String& ntpserver);
            void EpochUpdate(time_t epochtime) { mEpochTime = epochtime; }
            void Control() { mEpochUpdater.Control(); }

            time_t NowEpochUTC() const { return mEpochTime; }
    };
}

#endif
