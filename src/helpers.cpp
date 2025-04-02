#include "headers.h"

SCREEN_STATE::SCREEN_STATE(CONTENT_STATE& content_state, int max_cols, int max_rows)
	: content_state(content_state), max_cols(max_cols), max_rows(max_rows)
{
	first_line = content_state.get_first_line_it();
	first_char = first_line->begin();
	screen_rows = SCREEN_ROWS(content_state, max_rows, max_cols);
	cursor_x = 0;
	cursor_y = 0;
}

void SCREEN_STATE::log_state()
{
	std::cerr << "first_line_in_page [" << &*first_line
		<< "] first_char_in_page [" << char_it_str(first_char, first_line);
}

PrintOperation SCREEN_STATE::handle_append(AppendCase acase)
{
	std::cout << "SCREEN_STATE::handle_append_char\n";
	log_state();

	if (acase == NewLine)
	{
		if (cursor_y == max_rows - 1)
		{
			scroll_down_first();

			last_line++;
			bool old_row_deletd = max_cols - cursor_x + get_char_offset(last_line) <= max_cols;
			if (last_line_offset_rows > 0 && old_row_deletd)
			{
				last_line_offset_rows--;
			}

			if (last_line_offset_rows > 0)
			{
				last_char = std::next(last_line->begin(), max_cols);
			}
			else
			{
				last_char = last_line->end();
			}

			cursor_x = 0;
			log_state();
			return Screen;
		}

		if (cursor_x == 0 && cursor_y == 0)
		{
			if (first_line_offset_rows > 0)
			{
				bool first_equals_last = first_line == last_line; 
				first_line++;
				first_char = first_line->begin();
				first_line_offset_rows = 0;
				if (first_equals_last)
				{
					last_line = first_line;
				}
				return None;
			}

			first_char = first_line->begin();
			if (is_screen_full())
			{
				scroll_up_last();
			}
			else
			{
				rows_filled++;
			}

			cursor_y++;
			return Screen;
		}

		bool new_row_added = did_newline_add_row(content_state.get_line_it());
		if (std::next(last_line) == content_state.get_line_it())
		{
			last_line++;
			if (new_row_added)
			{
				if (is_screen_full())
				{
					last_line_offset_rows++;
				}
				else
				{
					rows_filled++;
				}
			}

			if (last_line_offset_rows > 0)
			{
			}

			cursor_x = 0;
			cursor_y++;
			return content_state.is_cursor_at_end() ? None : AfterCursor;
		}

		if (new_row_added)
		{
			if (is_screen_full())
			{
				scroll_up_last();
			}
			else
			{
				rows_filled++;
			}
		}

		cursor_x = 0;
		cursor_y++;

		log_state();
		return AfterCursor;
	}

	int offset = acase == AppendTab ? 4 : 1;
	if (cursor_x == max_cols && cursor_y == max_rows - 1)
	{
		scroll_down_first();

		if (get_char_offset(last_line) <= offset)
		{
			last_line_offset_rows++;
		}
		last_char = std::prev(last_char, offset);

		cursor_x = offset;
		log_state();
		return Screen;
	}

	if (cursor_x == 0 && cursor_y == 0)
	{
		first_char = std::prev(first_char, offset);
	}

	int rows_in_line = get_rows_in_line(content_state.get_line_it());
	int char_offset = get_char_offset(content_state.get_line_it());
	bool new_row_added = rows_in_line > 1 && char_offset <= offset;
	if (content_state.get_line_it() == last_line)
	{
		if (new_row_added)
		{
			if (is_screen_full())
			{
				last_line_offset_rows++;
			}
			else if (rows_filled < max_rows)
			{
				rows_filled++;
			}
		}

		if (last_line_offset_rows > 0)
		{
			last_char--;
		}

		cursor_x += offset;
		if (cursor_x > max_cols)
		{
			cursor_x = cursor_x - max_cols;
			cursor_y++;
		}

		return content_state.is_cursor_at_end() ?
			None :
			AfterCursor;
	}

	if (new_row_added)
	{
		if (is_screen_full())
		{
			scroll_up_last();
		}
		else
		{
			rows_filled++;
		}
	}

	cursor_x += offset;
	if (cursor_x > max_cols)
	{
		cursor_x = cursor_x - max_cols;
		cursor_y++;
	}

	log_state();
	return AfterCursor;
}

bool SCREEN_STATE::did_newline_add_row(LINES::const_iterator line)
{
	int prev_line_size = std::prev(line)->size();
	int old_rows = get_rows_in_line(line->size() + prev_line_size);
	int new_rows = get_rows_in_line(prev_line_size) + get_rows_in_line(line);
	return new_rows > old_rows;
}

bool SCREEN_STATE::is_screen_full()
{
	return rows_filled == max_rows;
}

PrintOperation SCREEN_STATE::handle_backspace(BACKSPACE_STATE bstate)
{
	BackspaceCase bcase = bstate.bcase;
	if (bcase == NoContent)
	{
		return None;
	}

	if (bcase == LineStart)
	{
		int prev_line_size = bstate.prev_line_size;
		int prev_line_offset = get_char_offset(prev_line_size);
		int old_line_size = bstate.new_line->size() - prev_line_size;
		int old_line_offset = get_char_offset(old_line_size);
		bool old_row_deleted = prev_line_offset + old_line_offset <= max_cols;
		if (bstate.deleted_line == first_line)
		{
			first_line = bstate.new_line;
			first_char = std::prev(content_state.get_char_it(), prev_line_offset);
			first_line_offset_rows = get_rows_in_line(first_line) - 1;

			if (!old_row_deleted)
			{
				if (is_screen_full())
				{
					scroll_up_last();
				}
				else
				{
					rows_filled++;
				}
			}

			cursor_x = prev_line_offset;
			return Screen;
		}

		if (bstate.deleted_line == last_line)
		{
			last_line = bstate.new_line;
			if (old_row_deleted)
			{
				if (last_line_offset_rows > 0)
				{
					last_line_offset_rows--;
					last_char = std::next(last_char, max_cols);
				}
				else if (last_line != content_state.get_last_line_it())
				{
					last_line++;
					last_line_offset_rows = get
				}
				{
					rows_filled--;
				}
			}

			cursor_y--;
			cursor_x = prev_line_offset;
			return content_state.is_cursor_at_end() ?
				None :
				AfterCursor;
		}

		if (old_row_deleted)
		{
			if (is_screen_full())
			{
				scroll_down_last();
			}
			else
			{
				rows_filled--;
			}
		}

		cursor_x = prev_line_offset;
		return content_state.is_cursor_at_end() ?
			None :
			AfterCursor;
	}
	
	int offset = bcase == BackspaceTab ? 4 : 1;
	if (cursor_y == 0 && cursor_x == 0)
	{
		scroll_up();
		cursor_x = max_cols - 1;
	}
}

SCREEN_STATE_ITERATOR SCREEN_STATE::get_iterator_at_start()
{
	return SCREEN_STATE_ITERATOR(first_char, first_line, last_char);
}

SCREEN_STATE_ITERATOR SCREEN_STATE::get_iterator_at_cursor()
{
	return SCREEN_STATE_ITERATOR(content_state.get_char_it(), content_state.get_line_it(), last_char);
}

SCREEN_STATE_ITERATOR::SCREEN_STATE_ITERATOR(
	CHARS::const_iterator ch_it,
	LINES::const_iterator line_it,
	CHARS::const_iterator last_char)
	:
		ch_it(ch_it),
		line_it(line_it),
		last_char(last_char) { }

char SCREEN_STATE_ITERATOR::next()
{
	if (ch_it == line_it->end())
	{
		line_it++;
		ch_it = line_it->begin();
		return '\n';
	}
	return *(ch_it++);
}

bool SCREEN_STATE_ITERATOR::is_at_end()
{
	return ch_it == last_char;
}


std::string get_string_content(LINES& contents, std::unordered_map<CHARS*, std::unordered_set<char*>>& tabs_start)
{
	std::ostringstream oss;
	LINES::iterator line_it = contents.begin();
	LINES::iterator end_prev = std::prev(contents.end());
	while (line_it != contents.end())
	{
		CHARS& line = *line_it;
		CHARS::iterator char_it = line.begin();
		while (char_it != line.end())
		{
			if (is_tab_start(tabs_start, line_it, char_it))
			{
				for (int i = 0; i < 4; i++)
				{
					oss << ' ';
				}
				char_it = std::next(char_it, 4);
				continue;
			}
			oss << *char_it;
		}
		if (line_it != end_prev)
		{
			oss << "\n";
		}
		++line_it;
	}
	return oss.str();
}

void save_contents_to_file(std::string& filename, std::string& contents)
{
	std::ofstream outfile(filename);
	if (!outfile)
	{
		throw std::runtime_error("Failed to open file");
	}
	outfile.clear();
	outfile << contents;
	outfile.close();
}

bool is_append_char(int ch)
{
	if (ch >= 32 && ch <= 126 || ch == '\n' || ch == '\t') return true;
	return false;
}

int handle_left_state(CONTENT_STATE& content_state)
{
	std::cerr << "handle_left_state\n";
	CHARS& line = *content_state.y_it;
	if (content_state.x_it == line.begin() && content_state.y_it == content_state.contents.begin())
	{
		std::cerr << "content_state.x_it [] == line.begin() && content_state.y_it [" << &(*content_state.y_it) << "] == content_state.contents.begin()\n";
		return -1;
	}
	if (content_state.x_it == line.begin())
	{
		std::cerr << "content_state.x_it [" << x_it_str(content_state.x_it, content_state) << "] == line.begin()\n";
		CHARS& line = *(--content_state.y_it);
		content_state.x_it = line.end();
		std::cerr << "x_it [" << x_it_str(content_state.x_it, content_state) << "], y_it [" << &(*content_state.y_it) << "], line.size() [" << line.size() << "]\n";
		std::cerr << "returning [" << line.size() << "]\n";
		return line.size();
	}
	if (is_tab_end(content_state, content_state.y_it, content_state.x_it))
	{
		for (int i = 0; i < 4; i++)
		{
			content_state.x_it--;
		}
		return -1;
	}
	content_state.x_it--;
	std::cerr << "x_it [" << x_it_str(content_state.x_it, content_state) << "], y_it [" << &(*content_state.y_it) << "], line.size() [" << line.size() << "]\n";
	return -1;
}

std::string get_string_from_it(
	CONTENT_STATE& content_state,
	LINES::iterator y_it_start,
	CHARS::iterator x_it_start,
	int max_size)
{
	std::ostringstream oss;
	LINES::iterator y_it = y_it_start;
	CHARS::iterator x_it = x_it_start;
	LINES::iterator y_it_end = content_state.contents.end();
	int size = 0;
	while (y_it != y_it_end)
	{
		CHARS& line = *y_it;
		if (y_it != y_it_start)
		{
			x_it = line.begin();
		}
		while (x_it != line.end() && size <= max_size)
		{
			if (is_tab_start(content_state, y_it, x_it))
			{
				int offset = std::min(max_size - size, 4);
				for (int i = 0; i < offset; i++)
				{
					oss << ' ';
				}
				size += offset;
				x_it = std::next(x_it, offset);
				continue;
			}
			oss << *x_it;
			size++;
			x_it++;
		}
		if (size >= max_size)
		{
			break;
		}
		if (y_it != std::prev(y_it_end))
		{
			oss << "\n";
			size++;
		}
		y_it++;
	}
	return oss.str();
}
