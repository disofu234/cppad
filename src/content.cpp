#include "content.h"
#include "list_chars.h"
#include "content_cursor.h"

template<typename CHARS_T>
void initialize_content(CONTENT_T<CHARS_T>& content, std::istream& input)
{
	content.push_back(LINE_T<CHARS_T>());
	CONTENT_CURSOR_T<CHARS_T> insert_cursor(content);
	char c;
	while (input.get(c))
		insert_cursor.insert(c);
}

template<typename CHARS_T>
std::string get_content_string(CONTENT_T<CHARS_T>& content)
{
	std::ostringstream oss;
	CONTENT_CURSOR_T<CHARS_T> read_cursor(content);
	while (!read_cursor.is_at_contents_end())
	{
		RIGHT r = read_cursor.right();
		oss << r.ch;
	}
	return oss.str();
}

template void initialize_content<CHARS>(CONTENT_T<CHARS>&, std::istream&);
template void initialize_content<LIST_CHARS>(CONTENT_T<LIST_CHARS>&, std::istream&);
template std::string get_content_string<CHARS>(CONTENT_T<CHARS>&);
template std::string get_content_string<LIST_CHARS>(CONTENT_T<LIST_CHARS>&);
