#include "screen_cursor.h"
#include "tabs.h"
#include <stdexcept>

SCREEN_CURSOR::SCREEN_CURSOR(CSCREEN& screen) :
    x(0),
    y(0),
    screen(screen),
    cc(screen.content),
    cols(screen.get_cols()),
    rows(screen.get_rows()) {}

SCREEN_CURSOR::SCREEN_CURSOR(CSCREEN& screen, int first_row, int target_x, int target_y) :
    x(0),
    y(0),
    screen(screen),
    cc(screen.content),
    cols(screen.get_cols()),
    rows(screen.get_rows())
{
    if (cols <= 0 || rows <= 0)
    {
        throw std::out_of_range("screen dimensions must be positive");
    }

    if (first_row < 0 || target_x < 0 || target_y < 0)
    {
        throw std::out_of_range("screen cursor coordinates must be non-negative");
    }

    if (target_x >= cols || target_y >= rows)
    {
        throw std::out_of_range("screen cursor coordinates exceed screen bounds");
    }

    int current_row = 0;
    bool wrapped_previous = false;

    while (true)
    {
        if (current_row == first_row && y == target_y && x == target_x)
        {
            return;
        }

        RIGHT step = cc.right();
        if (step.ch == '\0')
        {
            throw std::out_of_range("requested screen cursor position is unreachable");
        }

        if (step.ch == '\n')
        {
            if (wrapped_previous)
            {
                wrapped_previous = false;
                continue;
            }

            if (!wrapped_previous && current_row == first_row && y == target_y && target_x > x)
            {
                throw std::out_of_range("target_x is too large for content");
            }

            if (y == rows - 1)
            {
                if (current_row == first_row)
                {
                    throw std::out_of_range("first_row is too large for content");
                }

                screen.scroll_down();
                current_row++;
                x = 0;
                continue;
            }

            if (current_row == first_row && y == target_y)
            {
                throw std::out_of_range("target_y is too large for content");
            }

            y++;
            x = 0;
            continue;
        }

        int start_x = x;
        x += step.width;
        if (current_row == first_row && y == target_y &&
            target_x > start_x && target_x < x)
        {
            throw std::out_of_range("target_x is too large for content");
        }
        while (x >= cols)
        {
            wrapped_previous = true;

            x -= cols;
            if (y == rows - 1)
            {
                if (current_row == first_row)
                {
                    throw std::out_of_range("first_row is too large for content");
                }

                screen.scroll_down();
                current_row++;
                continue;
            }

            if (current_row == first_row && y == target_y)
            {
                throw std::out_of_range("target_y is too large for content");
            }

            y++;
        }

        if (current_row == first_row && y == target_y && x > target_x)
        {
            throw std::out_of_range("target_x is too large for content");
        }
    }
}

void SCREEN_CURSOR::insert(char ch)
{
    bool is_first_modified = y == 0 && x == 0;
    bool was_at_line_start = cc.is_at_line_start();

    INSERT inserted = cc.insert(ch);

    if (is_first_modified)
    {
        screen.first.reset(cc);
        if (ch == '\n')
        {
            if (inserted.new_line == nullptr)
            {
                throw std::runtime_error("newline insertion missing iterator");
            }

            if (was_at_line_start)
            {
                screen.scroll_up();
            }
        }
        else
        {
            int spaces = inserted.width;
            for (int i = 0; i < spaces; i++)
            {
                screen.first.prev();
            }
        }
    }

    if (ch == '\n')
    {
        if (y == rows - 1)
        {
            screen.scroll_down();
        }
        else
        {
            y++;
        }
        x = 0;
        return;
    }

    int advance = inserted.width > 0 ? inserted.width : 1;
    x += advance;
    while (x > cols)
    {
        if (y == rows - 1)
        {
            screen.scroll_down();
        }
        else
        {
            y++;
        }

        x -= cols;
    }
}

int SCREEN_CURSOR::get_x() const
{
    return x;
}

int SCREEN_CURSOR::get_y() const
{
    return y;
}
