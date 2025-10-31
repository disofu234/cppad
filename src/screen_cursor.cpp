#include "screen_cursor.h"
#include "tabs.h"

SCREEN_CURSOR::SCREEN_CURSOR(CSCREEN& screen) :
    x(0),
    y(0),
    screen(screen),
    cc(screen.content),
    cols(screen.get_cols()),
    rows(screen.get_rows()) {}

SCREEN_CURSOR::SCREEN_CURSOR(CSCREEN& screen, int target_first_row, int target_x, int target_y) :
    x(0),
    y(0),
    screen(screen),
    cc(screen.content),
    cols(screen.get_cols()),
    rows(screen.get_rows())
{
    if (target_first_row < 0 || target_x < 0 || target_y < 0)
    {
        throw std::out_of_range("screen cursor coordinates must be non-negative");
    }

    if (target_x > cols || target_y >= rows)
    {
        throw std::out_of_range("screen cursor coordinates exceed screen bounds");
    }

    for (int i = 0; i < target_first_row; i++)
    {
        screen.scroll_down();
    }

    cc.reset(screen.first);
    if (cc.is_at_contents_end())
    {
        throw std::out_of_range("target position is invalid");
    }

    x += cc.spaces_x - screen.first.spaces_x;
    while (x > cols || (!cc.is_at_line_end() && x == cols))
    {
        x -= cols;
        y++;
    }

    while (y < target_y || x < target_x)
    {
        if (cc.is_at_contents_end())
        {
            throw std::out_of_range("target position is invalid");
        }

        RIGHT r = right();

        if (r.scrolls > 0)
        {
            throw std::out_of_range("target position is invalid");
        }
    }
    
    if (y != target_y || x != target_x)
    {
        throw std::out_of_range("target position is invalid");
    }
}

void SCREEN_CURSOR::insert(char ch)
{
    bool is_first_modified = y == 0 && x == 0;
    bool was_at_line_start = cc.is_at_line_start();

    INSERT inserted = cc.insert(ch);

    if (ch == '\n')
    {
        if (is_first_modified)
        {
            screen.first.reset(cc);
            if (y == rows - 1)
            {
                return;
            }
            screen.scroll_up();
            y++;
            return;
        }

        if (x == 0 && !was_at_line_start)
        {
            return;
        }

        x = 0;
        if (y == rows - 1)
        {
            screen.scroll_down();
            return;
        }

        y++;
        return;
    }

    if (is_first_modified)
    {
        screen.first.reset(cc);
        for (int i = 0; i < inserted.width; i++)
        {
            screen.first.prev();
        }
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
            screen.get_spaces_in_last_row(std::prev(cc.line_it));
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

            x = prev_row_width;
            for (int i = 0; i < x; i++)
            {
                screen.first.prev();
            }
            return;
        }

        if (prev_row_width == cols && !cc.is_at_line_end())
        {
            return;
        }

        x = prev_row_width;
        y--;
        if (x == 0 && y == 0)
        {
            screen.first.reset(cc);
        }
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

    if (x == 0)
    {
        if (y == 0)
        {
            screen.first.reset(cc);
            if (cc.is_at_contents_start())
            {
                return;
            }

            screen.scroll_up();
            if (cc.is_at_line_end() && !cc.is_at_line_start())
            {
                x = cols;
                return;
            }

            y++;
            return;
        }

        if (cc.is_at_line_end() && !cc.is_at_line_start())
        {
            x = cols;
            y--;
            return;
        }
    }
}

RIGHT SCREEN_CURSOR::right()
{
    RIGHT r = cc.right();
    r.scrolls = 0;
    if (r.ch == '\0')
    {
        return r;
    }

    if (r.ch == '\n')
    {
        x = 0;

        if (y == rows - 1)
        {
            screen.scroll_down();
            r.scrolls++;
            return r;
        }

        y++;
        return r;
    }

    x += r.width;
    while (x > cols || (!cc.is_at_line_end() && x == cols))
    {
        x -= cols;

        if (y == rows - 1)
        {
            screen.scroll_down();
            r.scrolls++;
            continue;
        }

        y++;
    }

    return r;
}

LEFT SCREEN_CURSOR::left()
{
    LEFT l = cc.left();
    l.scrolls = 0;
    if (l.ch == '\0')
    {
        return l;
    }

    if (l.ch == '\n')
    {
        x = screen.get_spaces_in_last_row(cc.line_it);

        if (y == 0)
        {
            screen.scroll_up();
            l.scrolls++;
            return l;
        }

        y--;
        return l;
    }

    x -= l.width;
    while (x < 0)
    {
        x += cols;

        if (y == 0)
        {
            screen.scroll_up();
            l.scrolls++;
            continue;
        }

        y--;
    }

    return l;
}

UP SCREEN_CURSOR::up()
{
    UP u;
    u.scrolls = 0;

    if (is_at_content_first_row())
    {
        return u;
    }

    int old_x = x;
    int old_y = y;
    int prev_y = old_y - 1;
    bool was_prev_row = false;
    while (true)
    {
        LEFT l = left();
        u.scrolls += l.scrolls;

        bool is_prev_row =
            y == prev_y && u.scrolls == 0 ||
            y == old_y && u.scrolls == 1;
        
        bool is_past_prev_row =
            y < prev_y ||
            y == prev_y && u.scrolls > 0 ||
            y == old_y && u.scrolls > 1;

        if (cc.is_at_contents_start() ||
            is_past_prev_row ||
            is_prev_row && x <= old_x)
        {
            bool should_backtrack = was_prev_row && !is_prev_row;
            if (should_backtrack)
            {
                right();
            }
            break;
        }

        was_prev_row = is_prev_row;
    }

    return u;
}

DOWN SCREEN_CURSOR::down()
{
    DOWN d;
    d.scrolls = 0;

    if (is_at_content_last_row())
    {
        return d;
    }
    
    int old_x = x;
    int old_y = y;
    int next_y = old_y + 1;
    bool was_next_row = false;
    while (true)
    {
        RIGHT r = right();
        d.scrolls += r.scrolls;

        bool is_next_row =
            y == next_y && d.scrolls == 0 ||
            y == old_y && d.scrolls == 1;
        
        bool is_past_next_row =
            y > next_y ||
            y == next_y && d.scrolls > 0 ||
            y == old_y && d.scrolls > 1;

        if (cc.is_at_contents_end() ||
            is_past_next_row ||
            is_next_row && x >= old_x)
        {
            bool should_backtrack = was_next_row && (!is_next_row || x > old_x);
            if (should_backtrack)
            {
                left();
            }
            break;
        }

        was_next_row = is_next_row;
    }

    return d;
}

int SCREEN_CURSOR::get_x() const
{
    return x;
}

int SCREEN_CURSOR::get_y() const
{
    return y;
}

bool SCREEN_CURSOR::is_at_last_row_in_line() const
{
    return get_row_in_line() == get_num_rows_in_line() - 1;
}

bool SCREEN_CURSOR::is_at_first_row_in_line() const
{
    return get_row_in_line() == 0;
}

bool SCREEN_CURSOR::is_at_content_first_row() const
{
    return cc.is_first_line() && is_at_first_row_in_line();
}

bool SCREEN_CURSOR::is_at_content_last_row() const
{
    return cc.is_last_line() && is_at_last_row_in_line();
}

int SCREEN_CURSOR::get_num_rows_in_line() const
{
    int size = cc.line_it->size();
    if (size == 0)
    {
        return 1;
    }

    if (size % cols == 0)
    {
        return size / cols;
    }

    return (size / cols) + 1;
}

int SCREEN_CURSOR::get_row_in_line() const
{
    if (cc.is_at_line_end())
    {
        return (cc.spaces_x - 1) / cols;
    }

    return cc.spaces_x / cols;
}