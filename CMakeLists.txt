cmake_minimum_required(VERSION 3.10)
project(cppad VERSION 1.0)
enable_testing()
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
add_executable(cppad src/main.cpp src/screen.cpp src/content.cpp)
include_directories(src)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
add_executable(tests tests/test_main.cpp tests/test_content.cpp tests/test_screen.cpp tests/test_helpers.cpp src/screen.cpp src/content.cpp)
target_link_libraries(tests gtest_main)
include(GoogleTest)
gtest_discover_tests(tests)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})
target_link_libraries(cppad ${CURSES_LIBRARIES})
