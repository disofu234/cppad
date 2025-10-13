#include <string>
#include <strstream>
#include "../src/headers.h"

#pragma once

constexpr int MAX_ROWS = 2;
constexpr int MAX_COLS = 4;

CONTENT initialize_content(std::string text);
CURSOR initialize_cursor(CONTENT& content);
CSCREEN initialize_screen(std::string text, int max_rows, int max_cols);
std::string print_iterator(SCREEN_ITERATOR it);
