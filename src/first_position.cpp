#include "first_position.h"

FIRST_POSITION::FIRST_POSITION(CSCREEN& screen) :
    POSITION(screen.content),
    screen(screen) {}

void FIRST_POSITION::next_row()
{
    if (is_at_contents_end())
    {
        return;
    }

    bool hit_line_break = false;
    for (int i = 0; i < screen.get_cols(); i++)
    {
        char ch = next();
        if (ch == '\n')
        {
            hit_line_break = true;
            break;
        }
    }

    if (!hit_line_break && is_at_line_end())
    {
        next();
    }
}

void FIRST_POSITION::prev_row()
{
    int backtrack = screen.get_cols();
    if (is_at_line_start())
    {
        prev();
        backtrack = screen.get_spaces_in_last_row(line_it);
    }

    for (int i = 0; i < backtrack; i++)
    {
        prev();
    }
}

char FIRST_POSITION::next()
{
    if (!is_tab())
    {
        return POSITION::next();
    }

    spaces_x++;
    tabs_x++;
    if (tabs_x > max_tabs_x())
    {
        char_it++;
        tabs_it++;

        chars_x++;
        tabs_x = 0;
    }

    return ' ';
}

char FIRST_POSITION::prev()
{
    if (!is_tab() && !is_after_tab())
    {
        return POSITION::prev();
    }

    if (is_after_tab())
    {
        char ch = get_char();

        char_it--;
        tabs_it--;

        chars_x--;
        spaces_x--;
        tabs_x = max_tabs_x();

        return ch;
    }

    spaces_x--;
    tabs_x--;
    if (tabs_x < tabs_it->prev_chars)
    {
        char_it--;
        chars_x--;
    }

    return ' ';
}

int FIRST_POSITION::max_tabs_x() const
{
    return tabs_it->prev_chars + line_it->tabs.spaces(tabs_it) - 1;
}