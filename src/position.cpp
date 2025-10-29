#include "position.h"
#include "screen.h"

POSITION::POSITION(CONTENT& content) :
    content(content),
    char_it(content.front().chars.begin()),
    line_it(content.begin()),
    tabs_it(content.front().tabs.begin()),
    chars_x(0),
    spaces_x(0),
    tabs_x(0) {}

POSITION::POSITION(CONTENT& content, LINE_IT line_iterator, int x) :
    content(content),
    char_it(line_iterator->chars.begin()),
    line_it(line_iterator),
    tabs_it(line_iterator->tabs.begin()),
    chars_x(0),
    spaces_x(0),
    tabs_x(0)
{
    if (x < 0)
    {
        throw std::out_of_range("x is out of bounds");
    }

    for (int i = 0; i < x; i++)
    {
        if (is_at_line_end())
        {
            throw std::out_of_range("x is out of bounds");
        }

        next();
    }
}

POSITION::POSITION(const POSITION& other) :
    content(other.content),
    char_it(other.char_it),
    line_it(other.line_it),
    tabs_it(other.tabs_it),
    chars_x(other.chars_x),
    spaces_x(other.spaces_x),
    tabs_x(other.tabs_x) {}

bool POSITION::is_first_line() const
{
    return line_it == content.begin();
}

bool POSITION::is_at_line_start() const
{
    return chars_x == 0;
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

bool POSITION::is_tab() const
{
    return get_char() == '\t';
}

bool POSITION::is_after_tab() const
{
    return !is_at_line_start() && tabs_x == 0;
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

        chars_x = 0;
        spaces_x = 0;
        tabs_x = 0;

        return '\n';
    }

    char ch = *char_it;
    if (is_tab())
    {
        int spaces = line_it->tabs.spaces(tabs_it);

        char_it++;
        tabs_it++;

        chars_x++;
        spaces_x += spaces;
        tabs_x = 0;

        return '\t';
    }

    char_it++;

    chars_x++;
    spaces_x++;
    tabs_x++;

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

        chars_x = line_it->chars.size();
        spaces_x = line_it->size();
        tabs_x = tabs_it->prev_chars;

        return '\n';
    }

    if (is_after_tab())
    {
        char_it--;
        tabs_it--;

        chars_x--;
        spaces_x -= line_it->tabs.spaces(tabs_it);
        tabs_x = tabs_it->prev_chars;

        return '\t';
    }

    char_it--;

    chars_x--;
    spaces_x--;
    tabs_x--;

    return get_char();
}

void POSITION::reset(const POSITION& other)
{
    line_it = other.line_it;
    char_it = other.char_it;
    tabs_it = other.tabs_it;
    chars_x = other.chars_x;
    spaces_x = other.spaces_x;
    tabs_x = other.tabs_x;

    if (tabs_x > tabs_it->prev_chars)
    {
        int diff = tabs_x - tabs_it->prev_chars;

        char_it++;
        tabs_it++;

        chars_x++;
        spaces_x += line_it->tabs.spaces(std::prev(tabs_it)) - diff;
        tabs_x = 0;
    }
}

bool POSITION::operator==(const POSITION& other) const
{
    return char_it == other.char_it;
}

bool POSITION::operator!=(const POSITION& other) const
{
    return !(*this == other);
}