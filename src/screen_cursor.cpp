#include "screen_cursor.h"
#include "tabs.h"
#include <stdexcept>
#include <iterator>

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
    if (first_row < 0 || target_x < 0 || target_y < 0)
    {
        throw std::out_of_range("screen cursor coordinates must be non-negative");
    }

    if (target_x > cols || target_y >= rows)
    {
        throw std::out_of_range("screen cursor coordinates exceed screen bounds");
    }

    int current_row = 0;
    while (current_row != first_row)
    {
        if (cc.is_at_contents_end())
        {
            throw std::out_of_range("target position is invalid");
        }

        if (right())
        {
            current_row++;
        }

        bool invalid_position = current_row > first_row;
        if (invalid_position)
        {
            throw std::out_of_range("target position is invalid");
        }
    }

    // At this point either we've scrolled and y is at the last row, or there was
    // no scrolling and x and y are both zero.
    if (current_row > 0 && y > target_y)
    {
        while (!(y == target_y && x == target_x))
        {
            if (cc.is_at_contents_start())
            {
                throw std::out_of_range("target position is invalid");
            }

            if (left())
            {
                throw std::out_of_range("target position is invalid");
            }

            bool invalid_position =
                y < target_y ||
                (y == target_y && x < target_x);
            
            if (invalid_position)
            {
                throw std::out_of_range("target position is invalid");
            }
        }

        return;
    }

    while (!(y == target_y && x == target_x))
    {
        if (cc.is_at_contents_end())
        {
            throw std::out_of_range("target position is invalid");
        }

        if (right())
        {
            throw std::out_of_range("target position is invalid");
        }

        bool invalid_position =
            y > target_y ||
            (y == target_y && x > target_x);
        
        if (invalid_position)
        {
            throw std::out_of_range("target position is invalid");
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
        x = 0;

        if (y == rows - 1)
        {
            screen.scroll_down();
            return;
        }

        y++;
        return;
    }

    x += inserted.width;
    while (x > cols || (!cc.is_at_line_end() && x == cols))
    {
        x -= cols;

        if (y == rows - 1)
        {
            screen.scroll_down();
            continue;
        }

        y++;
    }
}

void SCREEN_CURSOR::backspace()
{
    int prev_row_width = 0;
    if (!cc.is_first_line())
    {
        prev_row_width =
            screen.get_spaces_in_last_row(std::prev(cc.get_line_it()));
    }

    BACKSPACE deleted = cc.backspace();
    if (deleted.ch == '\0')
    {
        return;
    }

    if (deleted.ch == '\n')
    {
        if (y == 0)
        {
            screen.first.reset(cc);
            if (prev_row_width == cols)
            {
                screen.scroll_up();
                y++;
                return;
            }

            for (int i = 0; i < prev_row_width; i++)
            {
                screen.first.prev();
            }
            x = prev_row_width;
            return;
        }

        if (prev_row_width == cols)
        {
            return;
        }

        x = prev_row_width;
        y--;
        return;
    }

    x -= deleted.width;
    while (x < 0)
    {
        x += cols;
        y--;
    }

    if (y < 0)
    {
        y = 0;
        screen.first.reset(cc);
        for (int i = 0; i < x; i++)
        {
            screen.first.prev();
        }

        return;
    }

    if (y == 0 && x == 0)
    {
        screen.first.reset(cc);
        if (cc.is_at_line_end())
        {
            screen.scroll_up();
            x = cols;
        }
    }
}

bool SCREEN_CURSOR::right()
{
    RIGHT step = cc.right();
    if (step.ch == '\0')
    {
        return false;
    }

    if (step.ch == '\n')
    {
        x = 0;

        if (y == rows - 1)
        {
            screen.scroll_down();
            return true;
        }

        y++;
        return false;
    }

    x += step.width;
    bool scrolled = false;
    while (x > cols || (!cc.is_at_line_end() && x == cols))
    {
        x -= cols;

        if (y == rows - 1)
        {
            screen.scroll_down();
            scrolled = true;
            continue;
        }

        y++;
    }

    return scrolled;
}

bool SCREEN_CURSOR::left()
{
    LEFT step = cc.left();
    if (step.ch == '\0')
    {
        return false;
    }

    if (step.ch == '\n')
    {
        x = screen.get_spaces_in_last_row(cc.get_line_it());

        if (y == 0)
        {
            screen.scroll_up();
            return true;
        }

        y--;
        return false;
    }

    x -= step.width;
    bool scrolled = false;
    while (x < 0)
    {
        x += cols;

        if (y == 0)
        {
            screen.scroll_up();
            scrolled = true;
            continue;
        }

        y--;
    }

    return scrolled;
}

int SCREEN_CURSOR::get_x() const
{
    return x;
}

int SCREEN_CURSOR::get_y() const
{
    return y;
}
