#include <cstdint>
#include <string>
#include "../ScheduleRunner.h"
#include "gtest/gtest.h

using namespace std;

class MockScheduleRunner : public ScheduleRunner {
public:
    MockScheduleRunner() :
        ScheduleRunner()
    {

    }
    virtual ~MockScheduleRunner() = default;

    void UpdateTime(uint16_t newTime)
    {
        m_time = newTime;
    }

    virtual uint16_t GetMinsSinceStartOfDay() const override
    {
        return m_time;
    }


protected:
    uint16_t m_time;
};

#include "gtest/gtest.h
TEST(ScheduleRunner, UpdateSchedule_DefaultCase) {
    string scheduleStr = "000000001010101011111111000011110000111111111111";
    MockScheduleRunner scheduleRunner;
    scheduleRunner.UpdateSchedule(scheduleStr);
    unsigned long long schedule = scheduleRunner.GetInternalScheduleRepresentation();
    EXPECT_EQ(schedule, 0xAAFF0F0FFF);
}

TEST(ScheduleRunner, UpdateSchedule) {
    string scheduleStr = "000000000000000000000000000000000000000000000001";
    MockScheduleRunner scheduleRunner;
    scheduleRunner.UpdateSchedule(scheduleStr);
    unsigned long long schedule = scheduleRunner.GetInternalScheduleRepresentation();
    EXPECT_EQ(schedule, 0x1);
}

TEST(ScheduleRunner, UpdateSchedule) {
    string scheduleStr = "11111111111111111111111111111111111111111111111";
    MockScheduleRunner scheduleRunner;
    scheduleRunner.UpdateSchedule(scheduleStr);
    unsigned long long schedule = scheduleRunner.GetInternalScheduleRepresentation();
    EXPECT_EQ(schedule, 0x7FFFFFFFFFFF);
}

TEST(ScheduleRunner, CheckSchedule) {
    MockScheduleRunner scheduleRunner;
    scheduleRunner.UpdateSchedule(scheduleStr);
}

TEST(ScheduleRunner, GetMinsToNextPeriodStart) {
    MockScheduleRunner scheduleRunner;
    scheduleRunner.UpdateSchedule(scheduleStr);
}

int main(int argc, char** argv) {
    // The following line must be executed to initialize Google Mock
    // (and Google Test) before running the tests.
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}