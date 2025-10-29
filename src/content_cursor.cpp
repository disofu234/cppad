#include "content_cursor.h"

CONTENT_CURSOR::CONTENT_CURSOR(CONTENT& content) : POSITION(content) {}
CONTENT_CURSOR::CONTENT_CURSOR(CONTENT& content, LINE_IT line_it, int chars_x)
    : POSITION(content, line_it, chars_x) {}

INSERT CONTENT_CURSOR::insert(char ch)
{
    INSERT result{};

    if (ch == '\n')
    {
        insert_line();
        result.width = 0;
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
    chars_x++;
    spaces_x += result.width;
    return result;
}

BACKSPACE CONTENT_CURSOR::backspace()
{
    BACKSPACE result{};

    if (is_at_contents_start())
    {
        result.ch = '\0';
        result.width = 0;
        return result;
    }

    if (is_at_line_start())
    {
        result.ch = '\n';
        result.width = 0;
        backspace_line();
        return result;
    }

    if (is_after_tab())
    {
        TABS_IT prev_tabs_it = std::prev(tabs_it);
        result.width = line_it->tabs.spaces(prev_tabs_it);
        int saved_tabs_x = prev_tabs_it->prev_chars;
        line_it->tabs.remove(tabs_it);
        tabs_it = prev_tabs_it;
        tabs_x = saved_tabs_x;
    }
    else
    {
        result.width = 1;
        line_it->tabs.add_prev_chars(tabs_it, -1);
        tabs_x--;
    }

    CHAR_IT prev_char_it = std::prev(char_it);
    result.ch = *prev_char_it;
    line_it->chars.erase(prev_char_it);
    chars_x--;
    spaces_x -= result.width;
    return result;
}

LEFT CONTENT_CURSOR::left()
{
    LEFT result{};
    result.ch = prev();
    result.width = 0;

    if (result.ch == '\0' || result.ch == '\n')
    {
        return result;
    }

    if (is_tab())
    {
        result.width = line_it->tabs.spaces(tabs_it);
        return result;
    }

    result.width = 1;
    return result;
}

RIGHT CONTENT_CURSOR::right()
{
    RIGHT result{};
    result.ch = next();
    result.width = 0;

    if (result.ch == '\0' || result.ch == '\n')
    {
        return result;
    }

    if (is_after_tab())
    {
        result.width = line_it->tabs.spaces(std::prev(tabs_it));
        return result;
    }

    result.width = 1;
    return result;
}

void CONTENT_CURSOR::insert_line()
{
    LINE_IT new_line_it = content.insert(std::next(line_it), LINE());

    CONTENT_CURSOR insert_cursor(content, new_line_it);
    while (!is_at_line_end())
    {
        right();
        BACKSPACE removed = backspace();
        char ch = removed.ch;
        insert_cursor.insert(ch);
    }

    line_it = new_line_it;
    char_it = new_line_it->chars.begin();
    tabs_it = new_line_it->tabs.begin();

    chars_x = 0;
    spaces_x = 0;
    tabs_x = 0;
}

void CONTENT_CURSOR::backspace_line()
{
    LINE_IT prev_line_it = std::prev(line_it);
    if (prev_line_it->chars.empty())
    {
        content.erase(prev_line_it);
        return;
    }

    CONTENT_CURSOR insert_cursor(content, prev_line_it, prev_line_it->chars.size());
    
    // Save the position where we want to end up after the operation
    POSITION saved_position = insert_cursor;
    saved_position.prev();

    while (!is_at_line_end())
    {
        right();
        BACKSPACE removed = backspace();
        char ch = removed.ch;
        insert_cursor.insert(ch);
    }

    content.erase(line_it);
    
    // Set cursor to the saved position
    saved_position.next();
    reset(saved_position);
}