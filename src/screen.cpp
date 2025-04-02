#include "headers.h"
#include <algorithm>
#include <fstream>
#include <iterator>

CSCREEN::CSCREEN(std::ifstream& file, int max_rows, int max_cols) :
	rows(),
	max_rows(max_rows),
	max_cols(max_cols),
	content_state(file),
	x(0),
	y(0),
	line_y(0),
	line_ind(0),
	rows_filled(0),
	rows_change(0)
{
	CLINES::const_iterator line_it = content_state.get_first_line_it();
	CLINES::const_iterator contents_end = std::next(content_state.get_last_line_it());
	std::cerr << "line_it: " << &(*line_it) << "\n";
	std::cerr << "contents_end: " << &(*contents_end) << "\n";
	std::cerr << "max_rows: " << max_rows << "\n";
	while (line_it != contents_end && rows_filled < max_rows)
	{
		std::cerr << "Adding " << &(*line_it) << "\n";
		LINE_ROWS line_rows = LINE_ROWS();
		line_rows.line = line_it;
		line_rows.rows_in_screen = get_rows_in_line(line_it);
		int rows_remaining = max_rows - rows_filled;
		if (line_rows.rows_in_screen > rows_remaining)
		{
			line_rows.rows_below_screen = line_rows.rows_in_screen - rows_remaining;
			line_rows.rows_in_screen = rows_remaining;
			line_rows.char_offset = max_cols;
			rows_filled = max_rows;
			rows.push_back(line_rows);
			break;
		}
		line_rows.char_offset = get_char_offset(line_it);
		rows_filled += line_rows.rows_in_screen;
		rows.push_back(line_rows);
		line_it++;
	}
}

void CSCREEN::append(CLINES::const_iterator line, int num_chars)
{
	ROWS::iterator line_rows = find(line);
	if (line_rows == rows.end())
	{
		throw std::runtime_error("Line not in screen.");
	}

	line_rows->char_offset += num_chars;
	while (line_rows->char_offset > max_cols)
	{
		line_rows->char_offset -= max_cols;
		line_rows->rows_in_screen++;
		rows_change++;
	}
}

void CSCREEN::append_line(CLINES::const_iterator line)
{
	std::cerr << "Appending line to rows: " << &(*line) << "\n";
	LINE_ROWS line_rows = LINE_ROWS();
	line_rows.line = line;
	line_rows.rows_in_screen = std::min(get_rows_in_line(line), max_rows);
	line_rows.char_offset = get_char_offset(line);
	
	ROWS::const_iterator insert_pos = find(std::next(line));
	rows.insert(insert_pos, line_rows);
	rows_change += line_rows.rows_in_screen;
}

void CSCREEN::backspace(CLINES::const_iterator line, int num_chars)
{
	ROWS::iterator line_rows = find(line);
	if (line_rows == rows.end())
	{
		throw std::runtime_error("Line not in screen.");
	}

	line_rows->char_offset -= num_chars;
	while (line_rows->char_offset < 0 || (line_rows->char_offset == 0 && line_rows->rows_in_screen > 1))
	{
		line_rows->char_offset += max_cols;
		line_rows->rows_in_screen--;
		rows_change--;
	}
}

void CSCREEN::backspace_line(int line_ind)
{
	if (line_ind >= rows.size())
	{
		throw std::runtime_error("Line not in screen.");
	}

	ROWS::iterator line_rows = std::next(rows.begin(), line_ind);
	rows_change -= line_rows->rows_in_screen;
	rows.erase(line_rows);
}

void CSCREEN::merge_with_next_line(int line_ind)
{
	LINE_ROWS& line_rows = rows[line_ind];
	ROWS::iterator next_line_rows = std::next(rows.begin(), line_ind + 1);
	line_rows.rows_in_screen += next_line_rows->rows_in_screen - 1;
	line_rows.rows_below_screen = next_line_rows->rows_below_screen;
	line_rows.char_offset += next_line_rows->char_offset;
	rows_change--;
	if (line_rows.char_offset > max_cols)
	{
		line_rows.rows_in_screen++;
		line_rows.char_offset -= max_cols;
		rows_change++;
	}
}

bool CSCREEN::commit()
{
	std::cerr << "committing " << rows_change << "\n";
	if (rows_change == 0) return false;

	if (rows_change > 0)
	{
		for (int i = 0; i < rows_change; i++)
		{
			add_row();
		}
	}
	else
	{
		for (int i = 0; i < -rows_change; i++)
		{
			remove_row();
		}
	}

	rows_change = 0;
	return true;
}

void CSCREEN::scroll_up()
{
	ROWS::iterator first_line_rows = rows.begin();
	if (first_line_rows->rows_above_screen > 0)
	{
		first_line_rows->rows_above_screen--;
		first_line_rows->rows_in_screen++;
	}
	else
	{
		CLINES::const_iterator first_line = first_line_rows->line;
		CLINES::const_iterator new_first_line = std::prev(first_line);
		LINE_ROWS new_first_line_rows = LINE_ROWS();
		new_first_line_rows.line = new_first_line;
		new_first_line_rows.rows_in_screen = 1;
		int rows_in_line = get_rows_in_line(new_first_line);
		new_first_line_rows.rows_above_screen = rows_in_line - 1;
		new_first_line_rows.char_offset = get_char_offset(new_first_line);
		rows.insert(rows.begin(), new_first_line_rows);
		line_ind++;
	}
	rows_change++;
}

void CSCREEN::scroll_down()
{
	ROWS::iterator first_line_rows = rows.begin();
	if (first_line_rows->rows_in_screen > 1)
	{
		first_line_rows->rows_above_screen++;
		first_line_rows->rows_in_screen--;
	}
	else
	{
		rows.erase(rows.begin());
	}
	rows_change--;
}

int CSCREEN::get_rows_in_line(CLINES::const_iterator line)
{
	int line_size = line->size();
	int rows_in_line = line_size / max_cols;
	if (rows_in_line == 0 || line_size % max_cols > 0)
	{
		rows_in_line++;
	}
	return rows_in_line;
}

int CSCREEN::get_char_offset(CLINES::const_iterator line)
{
	int line_size = line->size();
	int offset_mod = line_size % max_cols;
	if (line_size != 0 && offset_mod == 0)
	{
		return max_cols;
	}
	return offset_mod;
}

void CSCREEN::add_row()
{
	if (rows_filled < max_rows)
	{
		rows_filled++;
		return;
	}

	ROWS::iterator last_line_rows = std::prev(rows.end());
	if (last_line_rows->rows_in_screen > 1)
	{
		last_line_rows->rows_below_screen++;
		last_line_rows->rows_in_screen--;
		return;
	}

	rows.erase(last_line_rows);
}

void CSCREEN::remove_row()
{
	if (rows_filled < max_rows)
	{
		rows_filled--;
		return;
	}

	ROWS::iterator last_line_rows = std::prev(rows.end());
	CLINES::const_iterator last_line = last_line_rows->line;
	if (last_line_rows->rows_below_screen > 0)
	{
		last_line_rows->rows_below_screen--;
		last_line_rows->rows_in_screen++;
		return;
	}

	if (last_line == content_state.get_last_line_it())
	{
		rows_filled--;
		return;
	}

	CLINES::const_iterator next_line = std::next(last_line);
	LINE_ROWS next_line_rows = LINE_ROWS();
	next_line_rows.line = next_line;
	next_line_rows.rows_in_screen = 1;
	int rows_in_line = get_rows_in_line(next_line);
	next_line_rows.rows_below_screen = rows_in_line - 1;
	next_line_rows.char_offset = rows_in_line == 1 ?
		get_char_offset(next_line) :
		max_cols;
	rows.insert(rows.end(), next_line_rows);
}

ROWS::iterator CSCREEN::find(CLINES::const_iterator line)
{
	for (auto it = rows.begin(); it != rows.end(); it++)
	{
		LINE_ROWS& line_rows = *it;
		if (line_rows.line == line)
		{
			return it;
		}
	}
	return rows.end();
}

PrintOperation CSCREEN::handle_append(int ch)
{
	AppendCase acase = content_state.handle_append(ch);

	log_state();
	if (acase == NewLine)
	{
		CLINES::const_iterator line = content_state.get_line_it();
		backspace(std::prev(line), line->size());
		if (y == max_rows - 1 && !(x == 0 && line_y > 0))
		{
			scroll_down();
			commit();
			x = 0;
			line_ind++;
			log_state();
			return Screen;
		}

		append_line(line);
		commit();
		if (x == 0 && line_y > 0)
		{
			line_ind++;
			line_y = 0;
			log_state();
			return RemoveWrap;
		}

		x = 0;
		y++;
		line_ind++;
		line_y = 0;
		log_state();
		return AfterCursor;
	}

	int chars_added = acase == AppendTab ? 4 : 1;
	append(content_state.get_line_it(), chars_added);
	x += chars_added;
	if (x > max_cols)
	{
		x -= max_cols;
		if (y == max_rows - 1)
		{
			scroll_down();
			commit();
			log_state();
			return Screen;
		}

		y++;
		line_y++;
	}

	bool change = commit();
	return !change && content_state.is_cursor_at_line_end() ? None : AfterCursor;
}

PrintOperation CSCREEN::handle_backspace()
{
	BackspaceCase bcase = content_state.handle_backspace();
	log_state();
	if (bcase == NoContent)
	{
		return None;
	}

	if (bcase == LineStart)
	{
		if (y == 0)
		{
			backspace_line(line_ind);
			scroll_up();
			commit();
			refresh_x();
			log_state();
			return Screen;
		}

		merge_with_next_line(line_ind - 1);
	}

	int num_chars = bcase == BackspaceTab ? 4 : 1;
	backspace(content_state.get_line_it(), num_chars);
	x -= num_chars;
	if (x < 0)
	{
		x += max_cols;
		if (y == 0)
		{
			scroll_up();
			commit();
			log_state();
			return Screen;
		}

		y--;
		line_y--;
	}

	bool change = commit();
	log_state();
	return !change && content_state.is_cursor_at_line_end() ? None : AfterCursor;
}

SCREEN_ITERATOR CSCREEN::get_iterator_at_start()
{
	return SCREEN_ITERATOR(get_first_char(), get_first_line(), get_last_char());
}

SCREEN_ITERATOR CSCREEN::get_iterator_at_cursor()
{
	return SCREEN_ITERATOR(content_state.get_char_it(), content_state.get_line_it(), get_last_char());
}

void CSCREEN::log_state()
{
	std::cerr << "rows_filled: " << rows_filled << ", x: " << x << ", y: " << y << ", line_ind: " << line_ind << ", line_y: " << line_y << "\n";
	std::cerr << "rows:" << "\n";
	for (LINE_ROWS& line_row : rows)
	{
		std::cerr << "line: " << &(*(line_row.line))
			<< ", rows_above_screen: " << line_row.rows_above_screen
			<< ", rows_in_screen: " << line_row.rows_in_screen
			<< ", rows_below_screen: " << line_row.rows_below_screen
			<< ", char_offset: " << line_row.char_offset << "\n";
	}
}

PrintOperation CSCREEN::handle_left()
{
	LeftRightCase lrcase = content_state.handle_left();
	int n = lrcase == LeftRightTab ? 4 : 1;

	bool screen = false;
	for (int i = 0; i < n; i++)
	{
		if (y == 0 && x == 0)
		{
			scroll_up();
			screen = true;
			continue;
		}

		if (x > 0)
		{
			x--;
			continue;
		}

		if (line_y == 0)
		{
			line_ind--;
			LINE_ROWS& line_rows = rows[line_ind];
			x = line_rows.char_offset;
			y--;
			line_y = line_rows.rows_in_screen - 1;
			continue;
		}

		x = max_cols;
		y--;
		line_y--;
		continue;
	}

	return screen ? Screen : None;
}

void CSCREEN::left(int n)
{
	for (int i = 0; i < n; i++)
	{
		handle_left();
	}
}

PrintOperation CSCREEN::handle_right()
{
	LeftRightCase lrcase = content_state.handle_right();
	int n = lrcase == LeftRightTab ? 4 : 1;

	bool screen = false;
	for (int i = 0; i < n; i++)
	{
		if (y == max_rows - 1 && x == max_cols)
		{
			scroll_down();
			screen = true;
			continue;
		}

		LINE_ROWS& line_rows = rows[line_ind];
		if (line_y == line_rows.rows_in_screen - 1)
		{
			if (x < line_rows.char_offset)
			{
				x++;
				continue;
			}

			line_ind++;
			LINE_ROWS& line_rows = rows[line_ind];
			x = 0;
			y++;
			line_y = 0;
			continue;
		}

		if (x < max_cols - 1)
		{
			x++;
			continue;
		}

		x = 0;
		y++;
		line_y++;
		continue;
	}

	return screen ? Screen : None;
}

void CSCREEN::right(int n)
{
	std::cerr << "right\n";
	for (int i = 0; i < n; i++)
	{
		handle_right();
	}
	log_state();
}

int CSCREEN::get_x()
{
	return x;
}

int CSCREEN::get_y()
{
	return y;
}

CHARS::const_iterator CSCREEN::get_first_char()
{
	LINE_ROWS& line_rows = rows.front();
	CLINES::const_iterator first_line = line_rows.line;
	if (rows.size() == 1)
	{
		CHARS::const_iterator first_char = content_state.get_char_it();
		first_char = std::prev(first_char, x);
		first_char = std::prev(first_char, y * max_cols);
		std::cerr << "first_char: " << char_it_str(first_char, first_line) << "\n";
		return first_char;
	}

	CHARS::const_iterator first_char = first_line->end();
	first_char = std::prev(first_char, line_rows.char_offset);
	first_char = std::prev(first_char, (line_rows.rows_in_screen - 1) * max_cols);
	std::cerr << "first_char: " << char_it_str(first_char, first_line) << "\n";
	return first_char;
}

CHARS::const_iterator CSCREEN::get_last_char()
{
	std::cerr << "get_last_char\n";
	LINE_ROWS& line_rows = rows.back();
	CLINES::const_iterator last_line = line_rows.line;
	if (rows.size() == 1 && line_rows.rows_above_screen > 0)
	{
		CHARS::const_iterator last_char = content_state.get_char_it();
		int to_line_end = last_line->size() - content_state.get_x();
		int to_move = std::min(to_line_end, max_cols - x + (max_rows - y - 1) * max_cols);
		last_char = std::next(last_char, to_move);
		std::cerr << "last_char: " << char_it_str(last_char, last_line) << "\n";
		return last_char;
	}

	CHARS::const_iterator last_char = last_line->begin();
	last_char = std::next(last_char, (line_rows.rows_in_screen - 1) * max_cols);
	if (line_rows.rows_below_screen == 0)
	{
		last_char = std::next(last_char, line_rows.char_offset);
	}
	else
	{
		last_char = std::next(last_char, max_cols);
	}
	std::cerr << "last_char: " << char_it_str(last_char, last_line) << "\n";
	return last_char;
}

CLINES::const_iterator CSCREEN::get_first_line()
{
	return rows[0].line;
}

std::string CSCREEN::char_it_str(
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

void CSCREEN::refresh_x()
{
	if (content_state.get_x() == 0)
	{
		x = 0;
		return;
	}

	if (content_state.get_x() == max_cols)
	{
		x = max_cols;
		return;
	}

	x = content_state.get_x() % max_cols;
}

SCREEN_ITERATOR::SCREEN_ITERATOR(
	CHARS::const_iterator ch_it,
	CLINES::const_iterator line_it,
	CHARS::const_iterator last_char)
	:
		ch_it(ch_it),
		line_it(line_it),
		last_char(last_char) { }

char SCREEN_ITERATOR::next()
{
	if (ch_it == line_it->end())
	{
		line_it++;
		ch_it = line_it->begin();
		return '\n';
	}
	return *(ch_it++);
}

bool SCREEN_ITERATOR::is_at_end()
{
	return ch_it == last_char;
}
