#include "content_cursor.h"

int LINE::size() const
{
    return chars.size() - tabs.tab_count() + tabs.spaces();
}

CONTENT_CURSOR::CONTENT_CURSOR(CONTENT& content) :
    content(content),
    line_it(content.begin()),
    char_it(line_it->chars.begin()),
    x(0),
    tabs_it(line_it->tabs.begin()),
    tabs_x(0) {}

CONTENT_CURSOR::CONTENT_CURSOR(CONTENT& content, LINE_IT line_it, int x) :
    content(content),
    line_it(line_it),
    char_it(line_it->chars.begin()),
    x(0),
    tabs_it(line_it->tabs.begin()),
    tabs_x(0)
{
    if (x == line_it->chars.size())
    {
        this->x = line_it->chars.size();
        char_it = line_it->chars.end();
        tabs_it = std::prev(line_it->tabs.end());
        tabs_x = tabs_it->prev_chars;
        return;
    }

    for (int i = 0; i < x; i++)
    {
        next_ch();
    }
}

INSERT CONTENT_CURSOR::insert(char ch)
{
    INSERT result{};
    result.width = 0;
    result.new_line = nullptr;

    if (ch == '\n')
    {
        insert_line();
        result.new_line = &line_it;
        return result;
    }

    if (ch  == '\t')
    {
        line_it->tabs.insert(tabs_it, tabs_x);
        result.width = line_it->tabs.spaces(tabs_it);
        tabs_x = 0;
        tabs_it++;
    }
    else
    {
        result.width = 1;
        line_it->tabs.add_prev_chars(tabs_it, 1);
        tabs_x++;
    }

    line_it->chars.insert(char_it, ch);
    x++;
    return result;
}

char CONTENT_CURSOR::backspace()
{
    if (is_at_contents_start())
    {
        return '\0';
    }

    if (is_at_line_start())
    {
        backspace_line();
        return '\n';
    }

    if (is_tab_end())
    {
        TABS_IT prev_tabs_it = std::prev(tabs_it);
        int saved_tabs_x = prev_tabs_it->prev_chars;
        line_it->tabs.remove(tabs_it);
        tabs_it = prev_tabs_it;
        tabs_x = saved_tabs_x;
    }
    else
    {
        line_it->tabs.add_prev_chars(tabs_it, -1);
        tabs_x--;
    }

    CHAR_IT prev_char_it = std::prev(char_it);
    char ch = *prev_char_it;
    line_it->chars.erase(prev_char_it);
    x--;
    return ch;
}

char CONTENT_CURSOR::left()
{
    char ch = prev_ch();
    return ch == '\0' ? prev_line() : ch;
}

RIGHT CONTENT_CURSOR::right()
{
    if (is_at_line_end())
    {
        char nl = next_line();
        RIGHT r{nl, 0};
        return r;
    }

    if (is_tab_start())
    {
        int w = line_it->tabs.spaces(tabs_it);
        char ch = next_ch();
        RIGHT r{ch, w};
        return r;
    }

    char ch = next_ch();
    RIGHT r{ch, 1};
    return r;
}

LINE_IT CONTENT_CURSOR::get_line_it() const
{
    return line_it;
}

CHAR_IT CONTENT_CURSOR::get_char_it() const
{
    return char_it;
}

int CONTENT_CURSOR::get_x() const
{
    return x;
}

char CONTENT_CURSOR::get_char() const
{
    if (is_at_line_end())
    {
        return '\0';
    }

    return *char_it;
}

void CONTENT_CURSOR::insert_line()
{
    LINE_IT new_line_it = content.insert(std::next(line_it), LINE());

    CONTENT_CURSOR insert_cursor(content, new_line_it);
    while (!is_at_line_end())
    {
        right();
        char ch = backspace();
        insert_cursor.insert(ch);
    }

    next_line();
}

void CONTENT_CURSOR::backspace_line()
{
    LINE_IT prev_line_it = std::prev(line_it);
    CONTENT_CURSOR insert_cursor(content, prev_line_it, prev_line_it->chars.size());
    
    // Save the position where we want to end up after the operation
    CHAR_IT saved_char_it = std::prev(prev_line_it->chars.end());
    int saved_x = prev_line_it->chars.size();
    TABS_IT saved_tabs_it = std::prev(prev_line_it->tabs.end());
    int saved_tabs_x = saved_tabs_it->prev_chars;

    while (!is_at_line_end())
    {
        right();
        char ch = backspace();
        insert_cursor.insert(ch);
    }

    content.erase(line_it);
    
    // Set cursor to the saved position
    line_it = prev_line_it;
    char_it = std::next(saved_char_it);
    x = saved_x;
    tabs_it = saved_tabs_it;
    tabs_x = saved_tabs_x;
}

char CONTENT_CURSOR::next_ch()
{
    if (is_at_line_end())
    {
        return '\0';
    }

    if (is_tab_start())
    {
        int spaces = line_it->tabs.spaces(tabs_it);
        tabs_it++;
        tabs_x = 0;
    }
    else
    {
        tabs_x++;
    }

    char ch = *char_it;
    char_it++;
    x++;
    return ch;
}

char CONTENT_CURSOR::prev_ch()
{
    if (is_at_line_start())
    {
        return '\0';
    }
    
    if (is_tab_end())
    {
        (void)line_it->tabs.spaces(tabs_it);
        tabs_it--;
        tabs_x = tabs_it->prev_chars;
    }
    else
    {
        tabs_x--;
    }

    char_it--;
    x--;
    return *char_it;
}

char CONTENT_CURSOR::next_line()
{
    if (is_last_line())
    {
        return '\0';
    }

    line_it++;
    char_it = line_it->chars.begin();
    x = 0;
    tabs_it = line_it->tabs.begin();
    tabs_x = 0;
    return '\n';
}

char CONTENT_CURSOR::prev_line()
{
    if (is_first_line())
    {
        return '\0';
    }

    line_it--;
    char_it = line_it->chars.end();
    x = line_it->chars.size();
    tabs_it = std::prev(line_it->tabs.end());
    tabs_x = tabs_it->prev_chars;
    return '\n';
}

bool CONTENT_CURSOR::is_first_line() const
{
    return line_it == content.begin();
}

bool CONTENT_CURSOR::is_at_line_start() const
{
    return char_it == line_it->chars.begin();
}

bool CONTENT_CURSOR::is_at_contents_start() const
{
    return is_first_line() && is_at_line_start();
}

bool CONTENT_CURSOR::is_last_line() const
{
    return line_it == std::prev(content.end());
}

bool CONTENT_CURSOR::is_at_line_end() const
{
    return char_it == line_it->chars.end();
}

bool CONTENT_CURSOR::is_at_contents_end() const
{
    return is_last_line() && is_at_line_end();
}

bool CONTENT_CURSOR::is_tab_start() const
{
    return *char_it == '\t';
}

bool CONTENT_CURSOR::is_tab_end() const
{
    return *std::prev(char_it) == '\t';
}

TABS_IT CONTENT_CURSOR::get_tabs_it() const
{
    return tabs_it;
}

int CONTENT_CURSOR::get_tabs_x() const
{
    return tabs_x;
}

void initialize_content(CONTENT& content, std::istream& input)
{
    content.push_back(LINE());
    CONTENT_CURSOR insert_cursor(content);
    char c;
    while (input.get(c))
    {
        insert_cursor.insert(c);
    }
}

std::string get_content_string(CONTENT& content)
{
    std::ostringstream oss;
    CONTENT_CURSOR read_cursor(content);
    while (!read_cursor.is_at_contents_end())
    {
        RIGHT r = read_cursor.right();
        oss << r.ch;
    }
    return oss.str();
}
