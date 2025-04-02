#include "headers.h"
#include <sstream>
#include <stdexcept>

CONTENT_STATE::CONTENT_STATE(std::ifstream& file)
{
	CLINES empty_contents = {{}};
	contents = std::make_shared<CLINES>(empty_contents);
	CLINES::iterator line_it = --contents->end();
	tabs_start = std::make_shared<std::unordered_map<const void*, LINE_TABS>>();

	char ch;
	while (file.get(ch))
	{
		if (ch == '\n')
		{
			contents->push_back(CHARS());
			line_it++;
			continue;
		}
		if (ch == '\t')
		{
			for (int i = 0; i < 4; i++)
			{
				line_it->push_back(' ');
			}
			add_tab(line_it, std::prev(line_it->end(), 4));
			continue;
		}
		line_it->push_back(ch);
	}

	y_it = contents->begin();
	x_it = y_it->begin();
	x = 0;
	y = 0;
	log_state();
}

void CONTENT_STATE::log_state()
{
	std::cerr << "x_it [" << char_it_str(x_it, y_it)
		<< "], y_it [" << &(*y_it)
		<< "], x [" << x
		<< "], y [" << y
		<< "], line.size() [" << y_it->size()
		<< "], contents.size() [" << contents->size() << "]\n";
}

std::string CONTENT_STATE::char_it_str(
	CHARS::const_iterator x_it,
	CLINES::const_iterator y_it)
{
	if (x_it == y_it->end())
	{
		return "end";
	}
	std::string s(1, *x_it);
	return s;
}

CLINES::const_iterator CONTENT_STATE::get_line_it()
{
	return y_it;
}

CHARS::const_iterator CONTENT_STATE::get_char_it()
{
	return x_it;
}

int CONTENT_STATE::get_x()
{
	return x;
}

int CONTENT_STATE::get_y()
{
	return y;
}

CLINES::const_iterator CONTENT_STATE::get_first_line_it()
{
	return contents->begin();
}

CLINES::const_iterator CONTENT_STATE::get_last_line_it()
{
	return std::prev(contents->end());
}

CHARS::const_iterator CONTENT_STATE::get_contents_end()
{
	return get_last_line_it()->end(); 
}

CHARS::const_iterator CONTENT_STATE::get_contents_begin()
{
	return get_first_line_it()->begin(); 
}

bool CONTENT_STATE::is_cursor_at_end()
{
	return x_it == get_contents_end();
}

bool CONTENT_STATE::is_cursor_at_line_end()
{
	return x_it == y_it->end();
}

AppendCase CONTENT_STATE::handle_append(int ch)
{
	std::cerr << "handle_append_char()\n";
	CHARS& line = *y_it;
	if (ch == '\n')
	{
		std::cerr << "ch == \\n\n";
		y_it = contents->insert(std::next(y_it), CHARS());
		CHARS& new_line = *y_it;
		if (x_it != line.end())
		{
			new_line.splice(new_line.end(), line, x_it, line.end());
		}
		x_it = new_line.begin();
		y++;
		x = 0;
		log_state();
		return NewLine;
	}
	if (ch == '\t')
	{
		std::cerr << "ch == \\t\n";
		for (int i = 0; i < 4; i++)
		{
			line.insert(x_it, ' ');
		}
		add_tab(y_it, std::prev(x_it, 4));
		x += 4;
		log_state();
		return AppendTab;
	}
	line.insert(x_it, ch);
	x++;
	log_state();
	return AppendNormal;
}

BackspaceCase CONTENT_STATE::handle_backspace()
{
	std::cerr << "handle_backspace\n";
	log_state();
	if (x_it == y_it->begin() && y_it == contents->begin())
	{
		std::cerr << "x_it == y_it->begin() && y_it == contents->begin()\n";
		log_state();
		return NoContent;
	}
	if (x_it == y_it->begin())
	{
		std::cerr << "x_it == y_it->begin()\n";
		CLINES::iterator prev_y_it = std::prev(y_it);
		int prev_line_size = prev_y_it->size();
		x = prev_line_size;
		merge_tab_lines(y_it, prev_y_it);
		x_it = --prev_y_it->end();
		prev_y_it->splice(prev_y_it->end(), *y_it);
		x_it++;
		contents->erase(y_it);
		CLINES::iterator deleted_y_it = y_it;
		y_it = prev_y_it;
		y--;
		log_state();
		return LineStart;
	}
	if (is_tab_end(y_it, x_it))
	{
		std::cerr << "is_tab_end(y_it, x_it)\n";
		remove_tab_end(y_it, x_it);
		std::cerr << "removed tab end\n";
		CHARS::const_iterator er_it = std::prev(x_it);
		CHARS::const_iterator prev_er_it = std::prev(er_it);
		for (int i = 0; i < 4; i++)
		{
			std::cerr << *er_it << "\n";
			y_it->erase(er_it);
			er_it = prev_er_it;
			if (i < 3)
			{
				prev_er_it = std::prev(prev_er_it);
			}
		}
		x -= 4;
		log_state();
		return BackspaceTab;
	}
	y_it->erase(std::prev(x_it));
	x--;
	log_state();
	return BackspaceNormal;
}

void CONTENT_STATE::add_tab(CLINES::const_iterator y_it, CHARS::const_iterator x_it)
{
	if (x_it == y_it->end())
	{
		throw std::runtime_error("Cannot add tab at end of list.");
	}

	const void* y_it_ptr = static_cast<const void*>(&(*y_it));
	const void* x_it_ptr = static_cast<const void*>(&(*x_it));
	std::cerr << "adding y: " << y_it_ptr << ", x: " << x_it_ptr << "\n";
	if (tabs_start->find(y_it_ptr) == tabs_start->end())
	{
		tabs_start->insert({y_it_ptr, LINE_TABS()});
	}
	LINE_TABS& tabs_in_line = tabs_start->at(y_it_ptr);
	if (tabs_in_line.find(x_it_ptr) != tabs_in_line.end())
	{
		return;
	}
	tabs_in_line.insert(x_it_ptr);
}

void CONTENT_STATE::merge_tab_lines(CLINES::const_iterator y_it, CLINES::const_iterator prev_y_it)
{
	const void* y_it_ptr = static_cast<const void*>(&(*y_it));
	const void* prev_y_it_ptr = static_cast<const void*>(&(*prev_y_it));
	std::cerr << "merging y: " << y_it_ptr << ", prev_y: " << prev_y_it_ptr << "\n";
	if (tabs_start->find(y_it_ptr) == tabs_start->end())
	{
		return;
	}
	LINE_TABS& tabs_in_line = tabs_start->at(y_it_ptr);
	LINE_TABS& prev_tabs_in_line = tabs_start->at(prev_y_it_ptr);
	prev_tabs_in_line.insert(tabs_in_line.begin(), tabs_in_line.end());
	tabs_start->erase(y_it_ptr);
}

void CONTENT_STATE::remove_tab_end(CLINES::const_iterator y_it, CHARS::const_iterator x_it)
{
	const void* y_it_ptr = static_cast<const void*>(&(*y_it));
	LINE_TABS& tabs_in_line = tabs_start->at(y_it_ptr);
	const void* x_it_ptr = static_cast<const void*>(&(*std::prev(x_it, 4)));
	std::cerr << "removing y: " << y_it_ptr << ", x: " << x_it_ptr << "\n";
	tabs_in_line.erase(x_it_ptr);
}

bool CONTENT_STATE::is_tab_start(CLINES::const_iterator y_it, CHARS::const_iterator x_it)
{
	const void* y_it_ptr = static_cast<const void*>(&(*y_it));
	if (tabs_start->find(y_it_ptr) == tabs_start->end())
	{
		return false;
	}
	LINE_TABS& tabs_in_line = tabs_start->at(y_it_ptr);
	const void* x_it_ptr = static_cast<const void*>(&(*x_it));
	std::cerr << "is_tab_start y: " << y_it_ptr << ", x: " << x_it_ptr << "\n";
	bool rc = tabs_in_line.find(x_it_ptr) != tabs_in_line.end();
	std::cerr << rc << "\n";
	return rc;
}

bool CONTENT_STATE::is_tab_end(CLINES::const_iterator y_it, CHARS::const_iterator x_it)
{
	CHARS::const_iterator tab_start = x_it;
	for (int i = 0; i < 4; i++)
	{
		if (tab_start == (y_it->begin()))
		{
			return false;
		}
		tab_start--;
	}
	std::cerr << "is_tab_end\n";
	return is_tab_start(y_it, tab_start);
}

std::string CONTENT_STATE::get_contents_string()
{
	std::stringstream ss;
	CLINES::iterator line = contents->begin();
	while (line != contents->end())
	{
		CHARS::iterator ch = line->begin();
		while (ch != line->end())
		{
			if (is_tab_start(line, ch))
			{
				ss << '\t';
				for (int i = 0; i  < 4; i++)
				{
					std::cerr << *ch << "\n";
					ch++;
				}
				continue;
			}
			ss << *ch;
			ch++;
			continue;
		}
		line++;
		if (line != contents->end())
		{
			ss << '\n';
		}
	}
	return ss.str();
}

LeftRightCase CONTENT_STATE::handle_left()
{
	if (x_it == y_it->begin())
	{
		if (y_it == contents->begin())
		{
			return LeftRightNormal;
		}
		y_it--;
		x_it = y_it->end();
		y--;
		x = y_it->size();
		return LeftRightNormal;
	}

	if (is_tab_end(y_it, x_it))
	{
		x_it = std::prev(x_it, 4);
		x -= 4;
		return LeftRightTab;
	}

	x_it--;
	x--;
	return LeftRightNormal;
}

LeftRightCase CONTENT_STATE::handle_right()
{
	if (x_it == y_it->end())
	{
		if (y_it == get_last_line_it())
		{
			return LeftRightNormal;
		}

		y_it++;
		x_it = y_it->begin();
		y++;
		x = 0;
		return LeftRightNormal;
	}

	if (is_tab_start(y_it, x_it))
	{
		x_it = std::next(x_it, 4);
		x += 4;
		return LeftRightTab;
	}

	x_it++;
	x++;
	log_state();
	return LeftRightNormal;
}

void CONTENT_STATE::left(int n)
{
	for (int i = 0; i < n; i++)
	{
		handle_left();
	}
}

void CONTENT_STATE::right(int n)
{
	for (int i = 0; i < n; i++)
	{
		handle_right();
	}
}

void CONTENT_STATE::up()
{
}

void CONTENT_STATE::down()
{
}
