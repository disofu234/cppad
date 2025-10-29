#include "content.h"
#include "content_cursor.h"

int LINE::size() const
{
    return chars.size() - tabs.tab_count() + tabs.spaces();
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