//
// Created by alex on 22/12/18.
//

#include "ScheduleRunner.h"

using namespace std;

ScheduleRunner::ScheduleRunner() :
    m_currentHeaterOutput(false)
{

}

void ScheduleRunner::StartScheduleRunner()
{

}

void ScheduleRunner::StopScheduleRunner()
{

}

void ScheduleRunner::UpdateSchedule(const string& scheduleStr)
{
    // Update m_schedule
}

uint16_t ScheduleRunner::GetCurrentTimeMins() const
{
    return 0;
}

bool ScheduleRunner::GetHeaterStatus(const uint16_t currentTimeInMinutes) const
{
    return false;
}