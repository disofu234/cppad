#include "test_helpers.h"

CONTENT initialize_content(std::string text)
{
	CONTENT content{{}};
	POSITION position(content, content.begin(), content.begin()->chars.begin(), 0);
	CURSOR cursor(content, position);
	for (char ch : text)
	{
		cursor.insert(ch);
	}
	return content;
}

CURSOR initialize_cursor(CONTENT& content)
{
	POSITION position(content, content.begin(), content.begin()->chars.begin(), 0);
	CURSOR cursor(content, position);
	return cursor;
}

CSCREEN initialize_screen(std::string text, int max_rows, int max_cols)
{
	std::ofstream file("test");
	file.clear();
	file << text;
	file.close();

	std::ifstream ifile("test");
	return CSCREEN(ifile, max_rows, max_cols);
}

std::string print_iterator(SCREEN_ITERATOR it)
{
	std::ostrstream oss;
	while (!it.is_at_end())
	{
		oss << std::string(1, it.next());
	}
	oss << std::ends;
	return oss.str();
}
