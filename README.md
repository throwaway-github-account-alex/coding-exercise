# coding-exercise
## Task
Controller receives schedules via STDIN and needs to switch the storage heater on and off according to the schedule
by printing either ON or OFF to STDOUT at the appropriate time.

Schedule is codified in string of zeros and ones (48, 1 for each 30 mins in 24 hours)

Can be updated at any time to receive updates
- Include instructions on how to build and run
- State assumptions explicitly
- Expected to be tested and TDD approach would be good
- written with maintainability in mind

## Initial thoughts

The schedule representation is easily represented as a bitfield. Look ups for whether the schedule requires the heater
to be on/off can be done in constant time O(1). Taking the current time, representing it as the total minutes passed for
 the present day and then dividing that value by the schedule precision (30 minutes).

The number of bits required to represent the schedule (48) would stop updates to the variable from being atomic on
32 bit systems, so if a multi-threaded design were to be used, some protection would be required in the form of mutexs
or an 'std::atomic' wrapper. Although consideration is usually made for all variables shared between threads.

Input from stdin is typically collected by std::getline, which blocks until a delimiter character is received, typically
 a '\n'. If this is the only way of collecting input from stdin, then a producer/consumer pattern might be appropriate,
 which would allow another thread to check the current schedule. If a timeout could be used with `std::getline`,
 then I could use the timeout to check for an update to the schedule every 30 mins, and no threading consideration would
  be required.

Looking into whether I can use `std::getline` with a timeout reveals that it's possible to poll the stdin stream and the
polling function can take a timeout parameter. However this functionality is platform specific (posix) - there's
probably a way to do it using `boost::asio`. There's also fgets (no timeout), select (posix), poll (posix), and other IO
methods. It's making the separate thread style look simpler.

### Producer / Consumer style

To simplify this, the process thread could be used for collecting schedule updates and a separate thread could be
spawned, to check the schedule. The main thread could then pass new schedules to this separate thread,
or kill the thread, if the user decides to quit.
+ Allows getLine to block the process thread, without stopping the schedule operation.
- Multi-threading consideration.

### Single event loop style
+ No consideration is required for threading, potentially simplifying the code.
- Methods to poll stdin, don't look platform independent, and looks like using a platform independent option is more
  complex than I would like.
- If the user takes a long time to input the schedule, it'll block the scheduler.
+ The specification given by Ben doesn't mention that it needs to be multi-platform.

I imagine with both of these patterns will be as equally unit testable, so long as the key components are separated out.
 Additionally, by overriding the time input it should be possible to check the behavior over an entire day.

## Assumptions
+ MVP refers to minimum viable product.
+ The schedule reads from left to right, with the start being the left point.
+ It's OK to stop the program with any invalid input.
+ It's implied, but not explicitly said, only output changes to the current heater state are written, rather than
  outputs every 30 minutes.

  If the hardware's anything like the hardware I work with, I would have thought a more
  explicit output as frequently as possible would be better...
+ '\n' delimits the input schedule string.
+ The initial heater state is OFF, however I could print OFF at start if the state were to be ambiguous.
+ Responsiveness to user input.
        I'll assume, faster is better.
        If the schedule updates, and the current 30 minute period changes heater status changes, the program should update the heater status immediately, rather than waiting until the next 30 minute period.

## How to build

This works for me in the command line, with llvm clang and cmake installed along with [CLion](https://www.jetbrains.com/clion/).

### Program

#### Build release
`cmake --build /coding-exercise/cmake-build-release --target all -- -j 2`

### Unit tests

#### Build release
`cmake --build /coding-exercise/unit-tests/cmake-build-release --target all -- -j 2`

## Project review

I think I've got it working correctly, it appears to be working on my system correctly, with different schedules.

![Expected result photo given input at 2018-12-22T18-13-54](https://raw.githubusercontent.com/throwaway-github-account-alex/coding-exercise/master/ProgramScreenshot.png)

I made the mistake of attempting to setup the eco-system (CLion, cmake and googletest) at the same time as starting the
test. I got unit test build working the morning after with some fresh eyes.

In summary, I pragmatically tested the code, debugging through it and checking the variable values, to try and keep within
self imposed deadlines. I ran a system test overnight (it was fine). Then (after I got my system sorted) I implemented the unit tests.

Here's the unit test output
> [==========] Running 11 tests from 1 test case.
> [----------] Global test environment set-up.
> [----------] 11 tests from ScheduleRunner
> [ RUN      ] ScheduleRunner.UpdateSchedule_1
> [       OK ] ScheduleRunner.UpdateSchedule_1 (0 ms)
> [ RUN      ] ScheduleRunner.UpdateSchedule_2
> [       OK ] ScheduleRunner.UpdateSchedule_2 (0 ms)
> [ RUN      ] ScheduleRunner.UpdateSchedule_3
> [       OK ] ScheduleRunner.UpdateSchedule_3 (0 ms)
> [ RUN      ] ScheduleRunner.CheckSchedule_bad_time
> [       OK ] ScheduleRunner.CheckSchedule_bad_time (0 ms)
> [ RUN      ] ScheduleRunner.CheckSchedule_HeaterUpdate
> [       OK ] ScheduleRunner.CheckSchedule_HeaterUpdate (0 ms)
> [ RUN      ] ScheduleRunner.CheckSchedule_UpdateHeaterState_OnlyUpdateOnChange
> [       OK ] ScheduleRunner.CheckSchedule_UpdateHeaterState_OnlyUpdateOnChange (0 ms)
> [ RUN      ] ScheduleRunner.GetMinsToNextPeriodStart_CurrentTimeIs0
> [       OK ] ScheduleRunner.GetMinsToNextPeriodStart_CurrentTimeIs0 (0 ms)
> [ RUN      ] ScheduleRunner.GetMinsToNextPeriodStart_CurrentTimeIs15
> [       OK ] ScheduleRunner.GetMinsToNextPeriodStart_CurrentTimeIs15 (0 ms)
> [ RUN      ] ScheduleRunner.GetMinsToNextPeriodStart_CurrentTimeIs29
> [       OK ] ScheduleRunner.GetMinsToNextPeriodStart_CurrentTimeIs29 (0 ms)
> [ RUN      ] ScheduleRunner.GetMinsToNextPeriodStart_CurrentTimeIs31
> [       OK ] ScheduleRunner.GetMinsToNextPeriodStart_CurrentTimeIs31 (0 ms)
> [ RUN      ] ScheduleRunner.GetMinsToNextPeriodStart_CurrentTimeIs1441
> [       OK ] ScheduleRunner.GetMinsToNextPeriodStart_CurrentTimeIs1441 (0 ms)
> [----------] 11 tests from ScheduleRunner (0 ms total)
>
> [----------] Global test environment tear-down
> [==========] 11 tests from 1 test case ran. (0 ms total)
> [  PASSED  ] 11 tests.


Merry Christmas chaps!

Thanks very much for considering me, I think I've enjoyed all of the parts of the interview, even the telephone stage,
despite not going particularly well.