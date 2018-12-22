//
// Created by alex on 22/12/18.
//

#ifndef CODING_EXERCISE_SCHEDULERUNNER_H
#define CODING_EXERCISE_SCHEDULERUNNER_H

#include <bitset>
#include <atomic>
#include <cstdint>
#include <string>
#include <thread>

// A class to manage the schedule.
class ScheduleRunner {
public: // Public interface
    ScheduleRunner();

    virtual ~ScheduleRunner();

    // Update the current schedule.
    // If True is returned, the update was successful. False otherwise.
    bool UpdateSchedule(const std::string& scheduleStr);

    // Check the current schedule to see if the heater needs switching on/off
    void CheckSchedule();

    // Get the number of minutes to the start of the next scheduler update.
    uint16_t GetMinsToNextPeriodStart() const;

protected: // internal methods
    // 1440 minutes in a day, so we can use a uint16_t (short) to represent this.
    // virtual to aid unit testing.
    // Returns a uint16_t to represent the number of minutes passed in the current day.
    virtual uint16_t GetMinsSinceStartOfDay() const;

    // Update the heater state output
    virtual void UpdateHeaterState(bool heaterState);

    // Get the heater status, separating this out from GetCurrentTimeMins, makes the code more unit-testable.
    bool GetHeaterStatus(std::uint16_t currentTimeInMinutes) const;

public: // public member variable
    // Updating this will change the resolution of the scheduler, for example, setting this to 15, will update the
    // expected bit size to 96 bits. Given more time, I'd consider putting this somewhere more obvious, like a constructor parameter.
    const static constexpr int MinutesInEachSchedulerPeriod = 30;
    // This is unlikely to change, unless something drastic happens.
    const static constexpr int MinutesInADay = 1440;
    // This is the resolution of the scheduler
    const static constexpr int ScheduleBitSize = MinutesInADay / MinutesInEachSchedulerPeriod; // = 48 bit size;

private:
    // The schedule (I've used a bitset here, but a unit64_t could be used as a bit field instead), however if the
    // if the resolution of the schedule changes, I can just modify it in a single location.
    std::bitset<ScheduleBitSize> m_schedule;
    // A variable to contain the current state, and indicate if the state has changed, when looking at the schedule.
    bool m_currentHeaterOutput;
};


#endif //CODING_EXERCISE_SCHEDULERUNNER_H
