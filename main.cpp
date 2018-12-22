#include <iostream>
#include "ScheduleRunner.h"

#include <iostream>
#include <string>
#include <poll.h> // I'm sure there would be a boost equivalent if required
#include <unistd.h> // For STDIN_FILENO.

using namespace std;

// this method waits for input from the user or a timeout, the return val indicates whether it was successful.
bool readInput(chrono::milliseconds timeout, string& stringOut) {
    const int fileDescriptorCount = 1;
    struct pollfd pollParams = {STDIN_FILENO, POLLIN, 0};
    int pollReturnVal = 0;
    pollReturnVal = poll(&pollParams, fileDescriptorCount, timeout.count());
    if (pollReturnVal == 1) {
        std::getline(std::cin, stringOut);
    } else if (pollReturnVal == -1) {
        // TODO Check for internal error codes from call
    }
    return (pollReturnVal == 1);
}

void MainLoop()
{
    ScheduleRunner scheduleRunner;
    string inputSchedule = "";
    bool programRunning = true;
    chrono::milliseconds timeout = 30s; // 30s timeout to recheck the schedule, seems more than reasonable. See todo below for improvement.
    while(programRunning) {
        bool updateAvailable = readInput(timeout, inputSchedule);
        if (updateAvailable == true) {
            const bool ScheduleSuccessful = scheduleRunner.UpdateSchedule(inputSchedule);
            programRunning = ScheduleSuccessful;

            // TODO possible optimisation to use fewer CPU cycles unnecessarily waking up.
            //uint16_t minutesToNextPeriod = scheduleRunner.GetMinsToNextPeriodStart();
            //timeout = chrono::duration_cast<chrono::milliseconds>(std::chrono::minutes(minutesToNextPeriod));
        }
        scheduleRunner.CheckSchedule();
    }
}

int main()
{
    MainLoop();
    return 0;
}