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
    string inputSchedule;
    bool programRunning = true;
    chrono::milliseconds timeout = 30s; // 30s timeout to recheck the schedule, seems more than reasonable. See todo below for improvement.
    while(programRunning) {
        bool updateAvailable = readInput(timeout, inputSchedule);
        if (updateAvailable) {
            const bool ScheduleSuccessful = scheduleRunner.UpdateSchedule(inputSchedule);
            programRunning = ScheduleSuccessful;

            // TODO possible optimisation to use fewer CPU cycles unnecessarily waking up.
            //  I would expect poll to actually be interrupt driven.
            //uint16_t minutesToNextPeriod = scheduleRunner.GetMinsToNextPeriodStart();
            //timeout = chrono::duration_cast<chrono::milliseconds>(std::chrono::minutes(minutesToNextPeriod));
        }
        scheduleRunner.CheckSchedule();
    }
}

int main()
{
    // Output blurb to instruct user on how to operate.
    cout << "Hi, welcome to the heater scheduling program." << endl;
    cout << "The scheduler will start working as soon as you enter a schedule." << endl << endl;
    cout << "The schedule is codified in a string of zeros and ones, " << endl <<
            "where each digit represents the storage heater state for a half-hour period, " << endl <<
            "and the whole string contains 48 digits (one digit for each 30 minute period in the following 24 hours)."
            << endl;
    cout << "Therefore, the expected input is to appear as \"000000001010101011111111000011110000111111111111\"," << endl;
    cout << " after entering the schedule, press 'enter'." << endl;
    cout << "To exit, type anything other than the expected string." << endl << endl;

    // Let's go!
    MainLoop();
    return 0;
}