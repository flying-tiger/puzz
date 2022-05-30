cmake_minimum_required(VERSION 3.14)
project(puzzle_solver)
enable_testing()

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        703bd9caab50b139428cea1aaff9974ebee5742e # release-1.10.0
)
FetchContent_MakeAvailable(googletest)
include(GoogleTest)

add_executable(solver-test solver-test.cpp)
target_link_libraries(solver-test gtest_main)
gtest_discover_tests(solver-test)