#include "position.h"
#include "screen.h"
#include <stdexcept>
#include <iterator>

POSITION::POSITION(CONTENT& content) :
    content(content),
    char_it(content.front().chars.begin()),
    line_it(content.begin()),
    tabs_it(content.front().tabs.begin()),
    tabs_x(0),
    x(0) {}

POSITION::POSITION(const POSITION& other) :
    content(other.content),
    char_it(other.char_it),
    line_it(other.line_it),
    tabs_it(other.tabs_it),
    tabs_x(other.tabs_x),
    x(other.x) {}

void POSITION::reset(const CONTENT_CURSOR& cc)
{
    line_it = cc.get_line_it();
    char_it = cc.get_char_it();
    tabs_it = cc.get_tabs_it();
    tabs_x = cc.get_tabs_x();

    int tab_spaces = 0;
    int num_tabs = 0;
    for (auto it = line_it->tabs.begin(); it != tabs_it && it != line_it->tabs.end(); ++it)
    {
        tab_spaces += line_it->tabs.spaces(it);
        if (line_it->tabs.is_tab(it))
        {
            num_tabs++;
        }
    }

    x = cc.get_x() + tab_spaces - num_tabs;
}

POSITION::POSITION(CONTENT& content, LINE_IT line_iterator, int x) :
    content(content),
    char_it(line_iterator->chars.begin()),
    line_it(line_iterator),
    tabs_it(line_iterator->tabs.begin()),
    tabs_x(0),
    x(0)
{
    if (x < 0 || x > line_it->size())
    {
        throw std::out_of_range("x is out of bounds for the line");
    }

    for (int i = 0; i < x; i++)
    {
        next();
    }
}

bool POSITION::is_first_line() const
{
    return line_it == content.begin();
}

bool POSITION::is_at_line_start() const
{
    return x == 0;
}

bool POSITION::is_at_contents_start() const
{
    return is_first_line() && is_at_line_start();
}

bool POSITION::is_last_line() const
{
    return line_it == std::prev(content.end());
}

bool POSITION::is_at_line_end() const
{
    return char_it == line_it->chars.end();
}

bool POSITION::is_at_contents_end() const
{
    return is_last_line() && is_at_line_end();
}

char POSITION::get_char() const
{
    if (is_at_line_end())
    {
        return '\0';
    }

    return *char_it;
}

char POSITION::next()
{
    if (is_at_contents_end())
    {
        return '\0';
    }

    if (is_at_line_end())
    {
        line_it++;
        char_it = line_it->chars.begin();
        tabs_it = line_it->tabs.begin();
        tabs_x = 0;
        x = 0;
        return '\n';
    }

    char ch = get_char();
    if (ch == '\t')
    {
        tabs_x++;
        x++;
        if (tabs_x > max_tabs_x())
        {
            char_it++;
            tabs_it++;
            tabs_x = 0;
            x++;
        }

        return ' ';
    }

    char_it++;
    tabs_x++;
    x++;
    return ch;
}

char POSITION::prev()
{
    if (is_at_contents_start())
    {
        return '\0';
    }

    if (is_at_line_start())
    {
        line_it--;
        char_it = line_it->chars.end();
        tabs_it = std::prev(line_it->tabs.end());
        tabs_x = max_tabs_x() + 1;
        x = line_it->chars.size();
        return '\n';
    }

    if (get_char() == '\t')
    {
        x--;
        tabs_x--;

        if (tabs_x < tabs_it->prev_chars)
        {
            char_it--;
        }

        if (tabs_x < 0)
        {
            tabs_it--;
            tabs_x = max_tabs_x();
        }

        return ' ';
    }

    char_it--;
    x--;
    tabs_x--;
    if (tabs_x < 0)
    {
        tabs_it--;
        tabs_x = max_tabs_x();
    }
    return get_char();
}

int POSITION::max_tabs_x() const
{
    return tabs_it->prev_chars + line_it->tabs.spaces(tabs_it) - 1;
}

bool POSITION::operator==(const POSITION& other) const
{
    return char_it == other.char_it;
}

bool POSITION::operator!=(const POSITION& other) const
{
    return !(*this == other);
}

int POSITION::distance_to(const POSITION& other) const
{
    if (line_it != other.line_it)
    {
        throw std::runtime_error("positions are not in the same line");
    }

    return other.x - x;
}

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
    if (is_at_contents_start())
    {
        return;
    }

    if (is_at_line_start())
    {
        prev();
        int backtrack = screen.get_spaces_in_last_row(line_it);
        for (int i = 0; i < backtrack; i++)
        {
            prev();
        }
        return;
    }

    for (int i = 0; i < screen.get_cols(); i++)
    {
        prev();
    }
}

void FIRST_POSITION::set(LINE_IT line_iterator, int x_offset)
{
    line_it = line_iterator;
    char_it = line_it->chars.begin();
    tabs_it = line_it->tabs.begin();
    tabs_x = 0;
    x = 0;

    for (int i = 0; i < x_offset; i++)
    {
        char ch = next();
        if (ch == '\0')
        {
            break;
        }
    }
}
