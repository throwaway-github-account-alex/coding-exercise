cmake_minimum_required(VERSION 3.13)
project(unit_tests)

set(CMAKE_CXX_STANDARD 14)

include_directories(..)

# Locate GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
add_executable(unit_tests main.cpp ../ScheduleRunner.cpp)

target_link_libraries(unit_tests ${GTEST_LIBRARIES} pthread)