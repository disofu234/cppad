cmake_minimum_required(VERSION 3.10)
project(cppad VERSION 1.0)
enable_testing()
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(test_tabs
  tests/test_tabs.cpp
  src/tabs.cpp)
target_link_libraries(test_tabs gtest_main)
target_include_directories(test_tabs PRIVATE ${CMAKE_SOURCE_DIR}/src)

add_executable(test_content_cursor
  tests/test_content_cursor.cpp
  src/tabs.cpp
  src/content_cursor.cpp)
target_link_libraries(test_content_cursor gtest_main)
target_include_directories(test_content_cursor PRIVATE ${CMAKE_SOURCE_DIR}/src)

add_executable(test_screen
  tests/test_screen.cpp
  src/tabs.cpp
  src/content_cursor.cpp
  src/screen.cpp
  src/position.cpp)
target_link_libraries(test_screen gtest_main)
target_include_directories(test_screen PRIVATE ${CMAKE_SOURCE_DIR}/src)

add_executable(test_screen_cursor
  tests/test_screen_cursor.cpp
  src/tabs.cpp
  src/content_cursor.cpp
  src/screen.cpp
  src/screen_cursor.cpp
  src/position.cpp)
target_link_libraries(test_screen_cursor gtest_main)
target_include_directories(test_screen_cursor PRIVATE ${CMAKE_SOURCE_DIR}/src)

include(GoogleTest)
gtest_discover_tests(test_tabs)
gtest_discover_tests(test_content_cursor)
gtest_discover_tests(test_screen)
gtest_discover_tests(test_screen_cursor)

add_executable(
  cppad
  src/main.cpp
  src/content_cursor.cpp
  src/screen.cpp
  src/screen_cursor.cpp
  src/position.cpp
  src/tabs.cpp)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})
target_link_libraries(cppad ${CURSES_LIBRARIES})