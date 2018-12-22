//
// Created by alex on 22/12/18.
//

#include <chrono>
#include <iostream>
#include <thread>
#include <cassert>
#include <algorithm>
#include "ScheduleRunner.h"

using namespace std;

ScheduleRunner::ScheduleRunner() :
    m_currentHeaterOutput(false)
{

}

ScheduleRunner::~ScheduleRunner()
{

};

bool ScheduleRunner::UpdateSchedule(const string& scheduleStr)
{
    bool successful = true;
    if (scheduleStr.length() != ScheduleBitSize)
    {
        successful = false;
    }

    // TODO Check for invalid input
    //for_each(scheduleStr.begin(), scheduleStr.end(), [](const char n) { if (n != '1' || n != '0') {throw std::invalid_argument("Unexpected input.");} });

    // TODO could be rewritten with a for each style using an iterator.
    if (successful) {
        std::bitset<ScheduleBitSize> newSchedule;
        for (uint8_t i = 0; i < scheduleStr.length(); ++i) {
            if (scheduleStr.at(i) == '1') {
                newSchedule.set(i);
            }
        }

        m_schedule = newSchedule;
    }
    return successful;
}

void ScheduleRunner::CheckSchedule()
{
    uint16_t timeInMinutes = GetMinsSinceStartOfDay();
    const bool HeaterStatus = GetHeaterStatus(timeInMinutes);
    if (HeaterStatus != m_currentHeaterOutput) {
        if (HeaterStatus) {
            cout << "ON" << endl;
        } else {
            cout << "OFF" << endl;
        }
        m_currentHeaterOutput = HeaterStatus;
    }
}

uint16_t ScheduleRunner::GetMinsSinceStartOfDay() const
{
    auto now = std::chrono::system_clock::now();
    time_t timeNow = std::chrono::system_clock::to_time_t(now);
    tm *date = std::localtime(&timeNow); // Create a date, and set the values to 0 to work out the difference from midnight.
    date->tm_hour = 0;
    date->tm_min = 0;
    date->tm_sec = 0;
    auto midnight = std::chrono::system_clock::from_time_t(std::mktime(date));
    auto diff = (now-midnight);
    return std::chrono::duration_cast<std::chrono::minutes>(diff).count();
}

uint16_t ScheduleRunner::GetMinsToNextPeriodStart() const
{
    // This is just the remainder of the count of minutes since the start of the day mod the scheduler period.
    uint16_t minutesSinceLastNewPeriod = GetMinsSinceStartOfDay() % MinutesInEachSchedulerPeriod;
    // With the minutes since the last new period, we can simply work out the remaining minutes.
    return MinutesInEachSchedulerPeriod - minutesSinceLastNewPeriod;
}

bool ScheduleRunner::GetHeaterStatus(const uint16_t currentTimeInMinutes) const
{
    // Work out the bit index for the timer, make sure we can't go over that index or divide by 0.
    uint16_t thirtySecondPeriodIndex = 0;
    if (currentTimeInMinutes == 0)
    {
        thirtySecondPeriodIndex = 0;
    } else {
        thirtySecondPeriodIndex = 30 / currentTimeInMinutes;
    }

    if (thirtySecondPeriodIndex >= ScheduleBitSize) // Shouldn't happen, regardless of leap seconds, etc.
    {
        throw std::logic_error("An error has been made, there appears to be more minutes in the day than possible.");
    }

    std::bitset<ScheduleBitSize> schedule = m_schedule;
    return schedule[thirtySecondPeriodIndex];
}