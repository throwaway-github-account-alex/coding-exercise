#include <cstdint>
#include <string>
#include "gtest/gtest.h"
#include "../ScheduleRunner.h"

using namespace std;

class MockScheduleRunner : public ScheduleRunner {
public:
    MockScheduleRunner() :
        ScheduleRunner(),
        m_time(0),
        m_heaterOutput("")
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

    virtual void UpdateHeaterState(bool heaterState) override
    {
        // todo consider changing this to consume the stdout stream instead, so I have fewer methods 'mocked'.
        m_heaterOutput += (heaterState ? "ON" : "OFF");
    }

    string getHeaterOutput()
    {
        return m_heaterOutput;
    }

protected:
    // My mocked time, I could have tried changing the system time instead, but this is less invasive.
    // Or mocking the std chrono library (might need more investigation).
    uint16_t m_time;
    string m_heaterOutput;
};

TEST(ScheduleRunner, UpdateSchedule_1) {
    MockScheduleRunner scheduleRunner;

    string scheduleStr = "000000000000000000000000000000000000000000000001";
    scheduleRunner.UpdateSchedule(scheduleStr);

    for (unsigned int i = 0; i < 1440; i += 30) {
        scheduleRunner.UpdateTime(i);
        scheduleRunner.CheckSchedule();
    }

    string expectedOutput = "ON"; // As the state doesn't change until it's switched on at the end.
    ASSERT_STREQ(scheduleRunner.getHeaterOutput().c_str(), expectedOutput.c_str()) <<
                                                                        "Schedule isn't producing expected result.";
}

TEST(ScheduleRunner, UpdateSchedule_2) {
    MockScheduleRunner scheduleRunner;

    string scheduleStr = "100000000000000000000000000000000000000000000000";
    scheduleRunner.UpdateSchedule(scheduleStr);

    for (unsigned int i = 0; i < 1440; ++i) {
        scheduleRunner.UpdateTime(i);
        scheduleRunner.CheckSchedule();
    }

    string expectedOutput = "ONOFF";
    ASSERT_STREQ(scheduleRunner.getHeaterOutput().c_str(), expectedOutput.c_str()) <<
                                                                        "Schedule isn't producing expected result.";
}


TEST(ScheduleRunner, UpdateSchedule_3) {
    MockScheduleRunner scheduleRunner;

    string scheduleStr = "000000001010101011111111000011110000111111111111";
    scheduleRunner.UpdateSchedule(scheduleStr);

    for (unsigned int i = 0; i < 1440; ++i) {
        scheduleRunner.UpdateTime(i);
        scheduleRunner.CheckSchedule();
    }

    string expectedOutput = "ONOFFONOFFONOFFONOFF"
                            "ON"
                            "OFF"
                            "ON"
                            "OFF"
                            "ON";
    ASSERT_STREQ(scheduleRunner.getHeaterOutput().c_str(), expectedOutput.c_str()) << "Schedule isn't producing expected result.";
}

TEST(ScheduleRunner, CheckSchedule_bad_time) {
    MockScheduleRunner scheduleRunner;

    string scheduleStr = "000000001010101011111111000011110000111111111111";
    scheduleRunner.UpdateSchedule(scheduleStr);
    scheduleRunner.UpdateTime(1440); // More minutes than possible in a day.

    ASSERT_ANY_THROW(scheduleRunner.CheckSchedule()) << "This should throw given the incorrect input";
}

TEST(ScheduleRunner, CheckSchedule_HeaterUpdate) {
    MockScheduleRunner scheduleRunner;

    string scheduleStr = "000000001010101011111111000011110000111111111111";
    scheduleRunner.UpdateSchedule(scheduleStr);
    scheduleRunner.UpdateTime(1440); // More minutes than possible in a day.

    ASSERT_ANY_THROW(scheduleRunner.CheckSchedule()) << "This should throw given the incorrect input";
}

TEST(ScheduleRunner, CheckSchedule_UpdateHeaterState_OnlyUpdateOnChange) {
    // Initial heater state is off
    MockScheduleRunner scheduleRunner;

    string scheduleStr = "100100000000000000000000000000000000000000000000";
    scheduleRunner.UpdateSchedule(scheduleStr);

    for (const uint16_t& time : {0, 30, 60, 90}) { // only cover 0, 30, 60, 90...I like this syntax!
        scheduleRunner.UpdateTime(time);
        scheduleRunner.CheckSchedule();
    }

    string expectedOutput = "ON"
                            "OFF" // Shouldn't be a duplicate OFF value
                            "ON";
    ASSERT_STREQ(scheduleRunner.getHeaterOutput().c_str(), expectedOutput.c_str()) << "Schedule isn't producing expected result.";
}

TEST(ScheduleRunner, GetMinsToNextPeriodStart_CurrentTimeIs0) {
    MockScheduleRunner scheduleRunner;

    scheduleRunner.UpdateTime(0);

    ASSERT_EQ(scheduleRunner.GetMinsToNextPeriodStart(), 30) << "Unexpected output, Expected 30 minutes remaining";
}

TEST(ScheduleRunner, GetMinsToNextPeriodStart_CurrentTimeIs15) {
    MockScheduleRunner scheduleRunner;

    scheduleRunner.UpdateTime(15);

    ASSERT_EQ(scheduleRunner.GetMinsToNextPeriodStart(), 15) << "Unexpected output, Expected 15 minutes remaining";
}

TEST(ScheduleRunner, GetMinsToNextPeriodStart_CurrentTimeIs29) {
    MockScheduleRunner scheduleRunner;

    scheduleRunner.UpdateTime(29);

    ASSERT_EQ(scheduleRunner.GetMinsToNextPeriodStart(), 1) << "Unexpected output, Expected 1 minutes remaining";
}

TEST(ScheduleRunner, GetMinsToNextPeriodStart_CurrentTimeIs31) {
    MockScheduleRunner scheduleRunner;

    scheduleRunner.UpdateTime(31);

    ASSERT_EQ(scheduleRunner.GetMinsToNextPeriodStart(), 29) << "Unexpected output, Expected 29 minutes remaining";
}

TEST(ScheduleRunner, GetMinsToNextPeriodStart_CurrentTimeIs1441) {
    MockScheduleRunner scheduleRunner;

    // This should still work, but would kill the application if the schedule happened to be looked up.
    scheduleRunner.UpdateTime(1441);

    ASSERT_EQ(scheduleRunner.GetMinsToNextPeriodStart(), 29) << "Unexpected output, Expected 29 minutes remaining";
}


int main(int argc, char** argv) {
    // The following line must be executed to initialize Google Mock
    // (and Google Test) before running the tests.
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}