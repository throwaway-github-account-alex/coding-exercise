//
// Created by alex on 22/12/18.
//

#ifndef CODING_EXERCISE_SCHEDULERUNNER_H
#define CODING_EXERCISE_SCHEDULERUNNER_H

#include <bitset>
#include <atomic>
#include <cstdint>
#include <string>

// A class to manage the schedule.
class ScheduleRunner {
public: // Public interface
    ScheduleRunner();

    virtual ~ScheduleRunner() = default;

    // Create the thread which will monitor the schedule
    void StartScheduleRunner();

    // Stop the schedule thread.
    void StopScheduleRunner();

    // Update the current schedule.
    void UpdateSchedule(const std::string& scheduleStr);

protected: // internal methods
    // 1440 minutes in a day, so we can use a uint16_t (short) to represent this.
    // virtual to aid unit testing.
    // Returns a uint16_t to represent the number of minutes passed in the current day.
    virtual uint16_t GetCurrentTimeMins() const;

    // Get the heater status, separating this out from GetCurrentTimeMins, makes the code more unit-testable.
    bool GetHeaterStatus(std::uint16_t currentTimeInMinutes) const;

private:
    // The schedule (I've used a bitset here, but a unit64_t could be used as a bit field instead), however if the
    // if the resolution of the schedule changes, I can just modify it in a single location.
    // TODO 48 should be referenced from a single location to ease maintenance.
    std::atomic<std::bitset<48>> m_schedule;
    // A variable to contain the current state, and indicate if the state has changed, when looking at the schedule.
    std::atomic_bool m_currentHeaterOutput;
};


#endif //CODING_EXERCISE_SCHEDULERUNNER_H
