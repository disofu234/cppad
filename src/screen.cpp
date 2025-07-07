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
	rows_filled(0)
{
	CLINES_CIT line_it = content_state.get_first_line_it();
	CLINES_CIT contents_end = std::next(content_state.get_last_line_it());
	while (line_it != contents_end && rows_filled < max_rows)
	{
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

PrintOperation CSCREEN::handle_append(int ch)
{
	SCREEN_CHANGE change("handle_append", this);
	content_state.handle_append(ch);
	if (ch == '\n')
	{
		CLINES_CIT line = content_state.get_line_it();
		backspace(line_ind, line->size(), change);
		if (y == max_rows - 1 && !(x == 0 && line_y > 0))
		{
			scroll_down(change);
			x = 0;
			line_ind++;
			return Screen;
		}

		append_line(line, line_ind + 1, change);
		if (x == 0 && line_y > 0)
		{
			line_ind++;
			line_y = 0;
			return RemoveWrap;
		}

		x = 0;
		y++;
		line_ind++;
		line_y = 0;
		return AfterCursor;
	}

	int chars_added = ch == '\t' ? 4 : 1;
	append(line_ind, chars_added, change);
	x += chars_added;
	if (x > max_cols)
	{
		x -= max_cols;
		if (y == max_rows - 1)
		{
			scroll_down(change);
			return Screen;
		}

		y++;
		line_y++;
	}

	bool after_cursor = !(content_state.is_cursor_at_end() ||
		(content_state.is_cursor_at_line_end() && change.rows_change));
	return after_cursor ?
		AfterCursor :
		(ch == '\t' ? Tab : None);
}

PrintOperation CSCREEN::handle_backspace()
{
	SCREEN_CHANGE change("handle_backspace", this);
	char ch = content_state.handle_backspace();
	if (ch == '\0')
	{
		return None;
	}

	if (ch == '\n')
	{
		if (y == 0)
		{
			backspace_line(line_ind, change);
			scroll_up(change);
			refresh_x();
			return Screen;
		}

		merge_with_next_line(line_ind - 1, change);
	}

	int num_chars = ch == '\t' ? 4 : 1;
	backspace(line_ind, num_chars, change);
	x -= num_chars;
	if (x < 0)
	{
		x += max_cols;
		if (y == 0)
		{
			scroll_up(change);
			return Screen;
		}

		y--;
		line_y--;
	}

	return !change.rows_changed() && content_state.is_cursor_at_line_end() ? None : AfterCursor;
}

PrintOperation CSCREEN::handle_left()
{
	SCREEN_CHANGE change("handle_left", this);
	LeftRightCase lrcase = content_state.handle_left();
	if (lrcase == LeftRightNone)
	{
		return None;
	}
	int n = lrcase == LeftRightTab ? 4 : 1;

	bool screen = false;
	for (int i = 0; i < n; i++)
	{
		if (x > 0)
		{
			x--;
			continue;
		}

		if (y == 0)
		{
			scroll_up(change);
			screen = true;
			y++;
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

		x = max_cols - 1;
		y--;
		line_y--;
		continue;
	}

	return screen ? Screen : None;
}

PrintOperation CSCREEN::handle_right()
{
	SCREEN_CHANGE change("handle_right", this);
	LeftRightCase lrcase = content_state.handle_right();
	if (lrcase == LeftRightNone)
	{
		return None;
	}
	int n = lrcase == LeftRightTab ? 4 : 1;

	bool screen = false;
	for (int i = 0; i < n; i++)
	{
		if (x < get_max_x_on_row())
		{
			x++;
			continue;
		}

		if (y == max_rows - 1)
		{
			scroll_down(change);
			screen = true;
			y--;
		}

		LINE_ROWS& line_rows = rows[line_ind];
		if (line_y == line_rows.rows_in_screen - 1)
		{
			line_ind++;
			LINE_ROWS& line_rows = rows[line_ind];
			x = 0;
			y++;
			line_y = 0;
			continue;
		}

		x = 0;
		y++;
		line_y++;
		continue;
	}

	return screen ? Screen : None;
}

int CSCREEN::get_rows_in_line(CLINES_CIT line)
{
	int line_size = line->size();
	int rows_in_line = line_size / max_cols;
	if (rows_in_line == 0 || line_size % max_cols > 0)
	{
		rows_in_line++;
	}
	return rows_in_line;
}

int CSCREEN::get_char_offset(CLINES_CIT line)
{
	int line_size = line->size();
	int offset_mod = line_size % max_cols;
	if (line_size != 0 && offset_mod == 0)
	{
		return max_cols;
	}
	return offset_mod;
}

SCREEN_ITERATOR CSCREEN::get_iterator_at_start()
{
	return SCREEN_ITERATOR(get_first_char(), get_first_line(), get_last_char());
}

SCREEN_ITERATOR CSCREEN::get_iterator_at_cursor()
{
	return SCREEN_ITERATOR(content_state.get_char_it(), content_state.get_line_it(), get_last_char());
}

void CSCREEN::append(int line_ind, int num_chars, SCREEN_CHANGE& change)
{
	LINE_ROWS& line_rows = rows[line_ind];
	line_rows.char_offset += num_chars;
	while (line_rows.char_offset > max_cols)
	{
		line_rows.char_offset -= max_cols;
		line_rows.rows_in_screen++;
		change.rows_change++;
	}
}

void CSCREEN::append_line(CLINES_CIT line, int line_ind, SCREEN_CHANGE& change)
{
	LINE_ROWS line_rows = LINE_ROWS();
	line_rows.line = line;
	line_rows.rows_in_screen = std::min(get_rows_in_line(line), max_rows);
	line_rows.char_offset = get_char_offset(line);
	
	ROWS::const_iterator insert_pos = std::next(rows.begin(), line_ind);
	rows.insert(insert_pos, line_rows);
	change.rows_change += line_rows.rows_in_screen;
}

void CSCREEN::backspace(int line_ind, int num_chars, SCREEN_CHANGE& change)
{
	LINE_ROWS& line_rows = rows[line_ind];
	line_rows.char_offset -= num_chars;
	while (line_rows.char_offset < 0 || (line_rows.char_offset == 0 && line_rows.rows_in_screen > 1))
	{
		line_rows.char_offset += max_cols;
		line_rows.rows_in_screen--;
		change.rows_change--;
	}
}

void CSCREEN::backspace_line(int line_ind, SCREEN_CHANGE& change)
{
	if (line_ind >= rows.size())
	{
		throw std::runtime_error("Line not in screen.");
	}

	ROWS::iterator line_rows = std::next(rows.begin(), line_ind);
	change.rows_change -= line_rows->rows_in_screen;
	rows.erase(line_rows);
}

void CSCREEN::merge_with_next_line(int line_ind, SCREEN_CHANGE& change)
{
	LINE_ROWS& line_rows = rows[line_ind];
	ROWS::iterator next_line_rows = std::next(rows.begin(), line_ind + 1);
	line_rows.rows_in_screen += next_line_rows->rows_in_screen - 1;
	line_rows.rows_below_screen = next_line_rows->rows_below_screen;
	line_rows.char_offset += next_line_rows->char_offset;
	change.rows_change--;
	if (line_rows.char_offset > max_cols)
	{
		line_rows.rows_in_screen++;
		line_rows.char_offset -= max_cols;
		change.rows_change++;
	}
}

void CSCREEN::commit(int rows_change)
{
	if (rows_change == 0) return;

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
}

void CSCREEN::scroll_up(SCREEN_CHANGE& change)
{
	ROWS::iterator first_line_rows = rows.begin();
	if (first_line_rows->rows_above_screen > 0)
	{
		first_line_rows->rows_above_screen--;
		first_line_rows->rows_in_screen++;
	}
	else
	{
		CLINES_CIT first_line = first_line_rows->line;
		CLINES_CIT new_first_line = std::prev(first_line);
		LINE_ROWS new_first_line_rows = LINE_ROWS();
		new_first_line_rows.line = new_first_line;
		new_first_line_rows.rows_in_screen = 1;
		int rows_in_line = get_rows_in_line(new_first_line);
		new_first_line_rows.rows_above_screen = rows_in_line - 1;
		new_first_line_rows.char_offset = get_char_offset(new_first_line);
		rows.insert(rows.begin(), new_first_line_rows);
		line_ind++;
	}
	change.rows_change++;
}

void CSCREEN::scroll_down(SCREEN_CHANGE& change)
{
	ROWS::iterator first_line_rows = rows.begin();
	if (first_line_rows->rows_in_screen > 1)
	{
		first_line_rows->rows_above_screen++;
		first_line_rows->rows_in_screen--;
	}
	else
	{
		rows.erase(first_line_rows);
		line_ind--;
	}
	change.rows_change--;
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
	CLINES_CIT last_line = last_line_rows->line;
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

	CLINES_CIT next_line = std::next(last_line);
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

void CSCREEN::log_state()
{
	std::cerr << "CONTENT:\n";
	content_state.log_state();

	std::cerr << "SCREEN:\n";
	std::cerr << "rows_filled: " << rows_filled
		<< ", x: " << x
		<< ", y: " << y
		<< ", line_ind: " << line_ind
		<< ", line_y: " << line_y
		<< ", first_char: " << *get_first_char()
		<< ", last_char: " << *get_last_char() << "\n";

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

void CSCREEN::log_start(std::string func)
{
	std::cerr << func << "\n";
	std::cerr << "Start state:\n";
	log_state();
}

void CSCREEN::log_end()
{
	std::cerr << "End state:\n";
	log_state();
	std::cerr << "\n";
}

void CSCREEN::left(int n)
{
	for (int i = 0; i < n; i++)
	{
		handle_left();
	}
}

void CSCREEN::right(int n)
{
	for (int i = 0; i < n; i++)
	{
		handle_right();
	}
}

int CSCREEN::get_x()
{
	return x;
}

int CSCREEN::get_y()
{
	return y;
}

CHARS_CIT CSCREEN::get_first_char()
{
	LINE_ROWS& line_rows = rows.front();
	CLINES_CIT first_line = line_rows.line;
	if (rows.size() == 1)
	{
		CHARS_CIT first_char = content_state.get_char_it();
		first_char = std::prev(first_char, x);
		first_char = std::prev(first_char, y * max_cols);
		return first_char;
	}

	CHARS_CIT first_char = first_line->end();
	first_char = std::prev(first_char, line_rows.char_offset);
	first_char = std::prev(first_char, (line_rows.rows_in_screen - 1) * max_cols);
	return first_char;
}

CHARS_CIT CSCREEN::get_last_char()
{
	LINE_ROWS& line_rows = rows.back();
	CLINES_CIT last_line = line_rows.line;
	if (rows.size() == 1 && line_rows.rows_above_screen > 0)
	{
		CHARS_CIT last_char = content_state.get_char_it();
		int to_line_end = last_line->size() - content_state.get_x();
		int to_move = std::min(to_line_end, max_cols - x + (max_rows - y - 1) * max_cols);
		last_char = std::next(last_char, to_move);
		return last_char;
	}

	CHARS_CIT last_char = last_line->begin();
	last_char = std::next(last_char, (line_rows.rows_in_screen - 1) * max_cols);
	if (line_rows.rows_below_screen == 0)
	{
		last_char = std::next(last_char, line_rows.char_offset);
	}
	else
	{
		last_char = std::next(last_char, max_cols);
	}
	return last_char;
}

CLINES_CIT CSCREEN::get_first_line()
{
	return rows[0].line;
}

std::string CSCREEN::char_it_str(
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

int CSCREEN::get_max_x_on_row()
{
	LINE_ROWS line_rows = rows[line_ind];
	return line_y == line_rows.rows_in_screen - 1 ? line_rows.char_offset : max_cols - 1;
}

SCREEN_ITERATOR::SCREEN_ITERATOR(
	CHARS_CIT ch_it,
	CLINES_CIT line_it,
	CHARS_CIT last_char) :
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

SCREEN_CHANGE::SCREEN_CHANGE(std::string func, CSCREEN* screen) :
	rows_change(0),
	screen(screen)
{
	screen->log_start(func);
}

SCREEN_CHANGE::~SCREEN_CHANGE()
{
	std::cerr << "rows_change: " << rows_change << "\n";
	screen->commit(rows_change);
	screen->log_end();
}

bool SCREEN_CHANGE::rows_changed()
{
	return rows_change != 0;
}
