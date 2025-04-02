#include <iostream>
#include <cstdio>
#include <fstream>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <string>
#include <list>
#include <sstream>
#include <filesystem>
#include <iterator>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <vector>

#ifndef HEADERS_H
#define HEADERS_H

enum BackspaceCase
{
	BackspaceNormal,
	NoContent,
	LineStart,
	BackspaceTab,
};

enum AppendCase
{
	AppendNormal,
	NewLine,
	AppendTab
};

enum LeftRightCase
{
	LeftRightNormal,
	LeftRightTab
};

enum PrintOperation
{
	None,
	Screen,
	AfterCursor,
	RemoveWrap
};

typedef std::list<char> CHARS;
typedef std::list<CHARS> CLINES;
typedef std::unordered_set<const void*> LINE_TABS;

class CONTENT_STATE
{
public:
	CONTENT_STATE(std::ifstream& file);

	CLINES::const_iterator get_line_it();
	CHARS::const_iterator get_char_it();

	int get_x();
	int get_y();

	CLINES::const_iterator get_first_line_it();
	CLINES::const_iterator get_last_line_it();

	CHARS::const_iterator get_contents_end();
	CHARS::const_iterator get_contents_begin();

	bool is_cursor_at_end();
	bool is_cursor_at_line_end();

	AppendCase handle_append(int ch);
	BackspaceCase handle_backspace();

	LeftRightCase handle_left();
	LeftRightCase handle_right();
	void up();
	void down();

	void left(int n);
	void right(int n);

	std::string get_contents_string();
private:
	std::shared_ptr<CLINES> contents;
	std::shared_ptr<std::unordered_map<const void*, LINE_TABS>> tabs_start;

	int x;
	int y;

	CHARS::iterator x_it;
	CLINES::iterator y_it;

	void add_tab(CLINES::const_iterator y_it, CHARS::const_iterator x_it);
	void merge_tab_lines(CLINES::const_iterator y_it, CLINES::const_iterator prev_y_it);
	void remove_tab_end(CLINES::const_iterator y_it, CHARS::const_iterator x_it);
	bool is_tab_start(CLINES::const_iterator y_it, CHARS::const_iterator x_it);
	bool is_tab_end(CLINES::const_iterator y_it, CHARS::const_iterator x_it);

	std::string char_it_str(CHARS::const_iterator x_it, CLINES::const_iterator y_it);
	void log_state();
};

class SCREEN_ITERATOR
{
public:
	SCREEN_ITERATOR(
		CHARS::const_iterator ch_it,
		CLINES::const_iterator line_it,
		CHARS::const_iterator last_char);

	bool is_at_end();
	char next();
private:
	CHARS::const_iterator ch_it;
	CLINES::const_iterator line_it;
	CHARS::const_iterator last_char;
};

struct LINE_ROWS
{
	CLINES::const_iterator line;
	int rows_above_screen;
	int rows_in_screen;
	int rows_below_screen;
	int char_offset;

	LINE_ROWS() :
		rows_above_screen(0),
		rows_in_screen(0),
		rows_below_screen(0),
		char_offset(0) {}
};

typedef std::vector<LINE_ROWS> ROWS;

class CSCREEN
{
private:
	ROWS rows;
	CONTENT_STATE content_state;

	int max_rows;
	int max_cols;

	int rows_filled;
	int x;
	int y;
	int line_ind;
	int line_y;

	int rows_change;

	void append(CLINES::const_iterator line, int num_chars);
	void append_line(CLINES::const_iterator line);
	void backspace(CLINES::const_iterator line, int num_chars);
	void backspace_line(int line_ind);
	void merge_with_next_line(int line_ind);
	void scroll_up();
	void scroll_down();

	bool commit();
	void add_row();
	void remove_row();

	int get_rows_in_line(CLINES::const_iterator line);
	int get_char_offset(CLINES::const_iterator line);

	ROWS::iterator find(CLINES::const_iterator line);

	CHARS::const_iterator get_first_char();
	CHARS::const_iterator get_last_char();
	CLINES::const_iterator get_first_line();

	void refresh_x();

	std::string char_it_str(CHARS::const_iterator x_it, CLINES::const_iterator y_it);
	void log_state();
public:
	CSCREEN(std::ifstream& file, int max_rows, int max_cols);

	SCREEN_ITERATOR get_iterator_at_cursor();
	SCREEN_ITERATOR get_iterator_at_start();
	
	PrintOperation handle_append(int ch);
	PrintOperation handle_backspace();
	PrintOperation handle_left();
	PrintOperation handle_right();

	void left(int n);
	void right(int n);

	int get_x();
	int get_y();
};

#endif
