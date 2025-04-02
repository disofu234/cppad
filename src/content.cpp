#include "headers.h"

CONTENT::CONTENT(std::ifstream& file) :
	contents({{}}),
	y_it(contents.begin()),
	x_it(y_it->begin()),
	x(0),
	y(0)
{
	char ch;
	while (file.get(ch))
	{
		handle_append(ch);
	}

	y_it = contents.begin();
	x_it = y_it->begin();
	x = 0;
	y = 0;
}

void CONTENT::log_state()
{
	std::cerr << "x_it [" << char_it_str(x_it, y_it)
		<< "], y_it [" << &(*y_it)
		<< "], x [" << x
		<< "], line.size() [" << y_it->size()
		<< "], contents.size() [" << contents.size() << "]\n";
}

std::string CONTENT::char_it_str(
	CHARS_CIT x_it,
	CLINES_CIT y_it)
{
	if (x_it == y_it->end())
	{
		return "end";
	}
	std::string s(1, *x_it);
	return s;
}

CLINES_CIT CONTENT::get_line_it()
{
	return y_it;
}

CHARS_CIT CONTENT::get_char_it()
{
	return x_it;
}

int CONTENT::get_x()
{
	return x;
}

int CONTENT::get_y()
{
	return y;
}

CLINES_CIT CONTENT::get_first_line_it()
{
	return contents.begin();
}

CLINES_CIT CONTENT::get_last_line_it()
{
	return std::prev(contents.end());
}

CHARS_CIT CONTENT::get_contents_end()
{
	return get_last_line_it()->end(); 
}

CHARS_CIT CONTENT::get_contents_begin()
{
	return get_first_line_it()->begin(); 
}

bool CONTENT::is_cursor_at_end()
{
	return x_it == get_contents_end();
}

bool CONTENT::is_cursor_at_line_end()
{
	return x_it == y_it->end();
}

void CONTENT::handle_append(int ch)
{
	handle_append(ch, y_it, x_it, &x);
}

void CONTENT::handle_append(
	char ch,
	CLINES_CIT y_it,
	CHARS_CIT x_it,
	int* x)
{
	switch (ch)
	{
		case '\n':
			handle_append_line(y_it, x_it, x);
			return;
		case '\t':
			handle_append_tab(y_it, x_it, x);
			return;
		default:
			handle_append_normal(ch, y_it, x_it, x);
			return;
	}
}

void CONTENT::handle_append_line(CLINES_CIT y_it, CHARS_CIT x_it, int* x)
{
	CLINES_CIT new_y_it = contents.insert(std::next(y_it), CHARS());
	CHARS_CIT x_it_start = x_it;

	int new_line_x = 0;
	std::string new_line_string = get_line_string(y_it, x_it, x);
	for (char ch : new_line_string)
	{
		int offset = 1;
		if (ch == '\t')
		{
			remove_tab(y_it, *x);
			offset = 4;
		}
		handle_append(ch, new_y_it, new_y_it->end(), &new_line_x);
		x_it += offset;
		x += offset;
	}

	y_it->erase(x_it_start, y_it->end());

	y_it = new_y_it;
	x_it = new_y_it->begin();
	y++;
	*x = 0;
}

void CONTENT::handle_append_tab(CLINES_CIT y_it, CHARS_CIT x_it, int* x)
{
	if (is_tab_stopped(y_it, tab_stop(*x)))
	{
		int spaces = get_tab_spaces(y_it, *x);
		change_tab_spaces(y_it, *x, -spaces);
		return;
	}

	int whitespace = 0;
	while (!is_tab_stop(*x))
	{
		y_it->insert(x_it, ' ');
		whitespace++;
		*x += 1;
	}

	CHARS* y_it_ptr = &(*y_it);
	if (tabs.find(y_it_ptr) == tabs.end())
	{
		tabs.insert({y_it_ptr, LINE_TABS()});
	}
	LINE_TABS& line_tabs = tabs[y_it_ptr];
	line_tabs[tab_stop(*x)] = whitespace;
}

void CONTENT::handle_append_normal(
	char ch,
	CLINES_CIT y_it,
	CHARS_CIT x_it,
	int* x)
{
	if (is_tab_stopped(y_it, tab_stop(*x)))
	{
		change_tab_spaces(y_it, *x, 1);
	}

	y_it->insert(x_it, ch);
	*x += 1;
}

void CONTENT::change_tab_spaces(CLINES_CIT y_it, int x, int change)
{
	int stop = tab_stop(x);
	CHARS* y_it_ptr = &(*y_it);
	LINE_TABS& line_tabs = tabs[y_it_ptr];
	line_tabs[stop] += change;
	int spaces = line_tabs[stop];
	if (spaces == 0)
	{
		while (line_tabs.find(stop) != line_tabs.end())
		{
			stop++;
		}
		line_tabs[stop] = TAB_SIZE;
	}
}

char CONTENT::handle_backspace()
{
	if (x_it == y_it->begin() && y_it == contents.begin())
	{
		return '\0';
	}

	if (x_it == y_it->begin())
	{
		handle_backspace_line();
		return '\n';
	}

	if (is_tab_end(y_it, tab_stop(x)))
	{
		handle_backspace_tab();
		return '\t';
	}

	return handle_backspace_normal();
}

void CONTENT::handle_backspace_line()
{
	CLINES_IT prev_y_it = std::prev(y_it);
	int x_offset = prev_y_it->size();

	int prev_x = 0;
	std::string line_string = get_line_string(y_it, x_it, x);
	x_it = --prev_y_it->end();
	for (char ch : line_string)
	{
		handle_append(ch, prev_y_it, prev_y_it->end(), &prev_x);
	}

	contents.erase(y_it);
	tabs.erase(&(*y_it));

	x_it++;
	y_it = prev_y_it;
	x = x_offset;
}

void CONTENT::handle_backspace_tab()
{
	int spaces = remove_tab(y_it, x - 1);
	x_it = std::prev(x_it, spaces);
	y_it->erase(x_it, x_it + spaces);
	x -= spaces;
}

char CONTENT::handle_backspace_normal()
{
	if (is_tab_stopped(y_it, tab_stop(x)))
	{
		change_tab_spaces(y_it, x, 1);
	}

	char ch = *std::prev(x_it);
	y_it->erase(std::prev(x_it));
	x--;
	return ch;
}

int CONTENT::remove_tab(CLINES_CIT y_it, int x)
{
	int spaces = get_tab_spaces(y_it, x);
	tabs[&(*y_it)].erase(tab_stop(x));
	return spaces;
}

int CONTENT::get_tab_spaces(CLINES_CIT y_it, int x)
{
	if (!is_tab_stopped(y_it, tab_stop(x)))
	{
		throw std::runtime_error("get_tab_spaces: tab stop not found");
	}

	return tabs[&(*y_it)][tab_stop(x)];
}

bool CONTENT::is_tab_start(CLINES_CIT y_it, int x)
{
	if (!is_tab_stopped(y_it, tab_stop(x)))
	{
		return false;
	}

	int spaces = get_tab_spaces(y_it, x);
	return (x % TAB_SIZE) == (TAB_SIZE - spaces);
}

bool CONTENT::is_tab_end(CLINES_CIT y_it, int x)
{
	return (x % TAB_SIZE) == 0 && is_tab_stopped(y_it, tab_stop(x - 1));
}

bool CONTENT::is_tab_stopped(CLINES_CIT y_it, int stop)
{
	CHARS* y_it_ptr = &(*y_it);
	if (tabs.find(y_it_ptr) == tabs.end())
	{
		return false;
	}
	
	LINE_TABS& line_tabs = tabs[y_it_ptr];
	return line_tabs.find(stop) != line_tabs.end();
}

std::string CONTENT::get_line_string(CLINES_CIT y_it, CHARS_CIT x_it, int x)
{
	std::stringstream ss;
	while (x_it != y_it->end())
	{
		bool is_tab = is_tab_start(y_it, x);
		char ch = is_tab ? '\t' : *x_it;
		int offset = is_tab ? 4 : 1;
		ss << ch;
		x_it += offset;
		x += offset;
	}
	return ss.str();
}

std::string CONTENT::get_string()
{
	std::stringstream ss;
	for (CLINES_CIT y_it = contents.begin(); y_it != contents.end(); y_it++)
	{
		ss << get_line_string(y_it, y_it->begin(), 0);
	}
	return ss.str();
}

LeftRightCase CONTENT::handle_left()
{
	if (x_it == y_it->begin())
	{
		if (y_it == contents.begin())
		{
			return LeftRightNone;
		}
		y_it--;
		x_it = y_it->end();
		x = y_it->size();
		return LeftRightNormal;
	}

	if (is_tab_end(y_it, x))
	{
		int spaces = get_tab_spaces(y_it, x - 1);
		x_it -= spaces;
		x -= spaces;
		return LeftRightTab;
	}

	x_it--;
	x--;
	return LeftRightNormal;
}

LeftRightCase CONTENT::handle_right()
{
	if (x_it == y_it->end())
	{
		if (y_it == get_last_line_it())
		{
			return LeftRightNone;
		}

		y_it++;
		x_it = y_it->begin();
		x = 0;
		return LeftRightLineEnd;
	}

	if (is_tab_start(y_it, x))
	{
		int spaces = get_tab_spaces(y_it, x);
		x_it += spaces;
		x += spaces;
		return LeftRightTab;
	}

	x_it++;
	x++;
	return LeftRightNormal;
}

void CONTENT::left(int n)
{
	for (int i = 0; i < n; i++)
	{
		handle_left();
	}
}

void CONTENT::right(int n)
{
	for (int i = 0; i < n; i++)
	{
		handle_right();
	}
}

void CONTENT::up()
{
}

void CONTENT::down()
{
}

bool CONTENT::is_tab_stop(int x_pos)
{
	return x_pos > 0 && x_pos % TAB_SIZE == 0;
}

int CONTENT::tab_stop(int i)
{
	return i / TAB_SIZE;
}