#include "test_helpers.h"

CONTENT initialize_content_state(std::string text)
{
	std::ofstream file("test");
	file.clear();
	file << text;
	file.close();

	std::ifstream ifile("test");
	return CONTENT(ifile);
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
