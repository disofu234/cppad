#include <string>
#include <strstream>
#include "../src/headers.h"

#pragma once

constexpr int MAX_ROWS = 2;
constexpr int MAX_COLS = 4;

CONTENT initialize_content_state(std::string text);
CSCREEN initialize_screen(std::string text, int max_rows, int max_cols);
std::string print_iterator(SCREEN_ITERATOR it);
