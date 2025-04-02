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

constexpr int TAB_SIZE = 8;

enum BackspaceCase
{
	BackspaceNormal,
	NoContent,
	LineStart,
	BackspaceTab,
};

enum LeftRightCase
{
	LeftRightNone,
	LeftRightNormal,
	LeftRightTab
};

enum PrintOperation
{
	None,
	Tab,
	Screen,
	AfterCursor,
	RemoveWrap
};

typedef std::list<char> CHARS;
typedef std::list<CHARS> CLINES;
typedef std::unordered_map<int, int> LINE_TABS;
typedef std::unordered_map<CHARS*, LINE_TABS> TABS;
typedef CLINES::iterator CLINES_IT;
typedef CLINES::const_iterator CLINES_CIT;
typedef CHARS::iterator CHARS_IT;
typedef CHARS::const_iterator CHARS_CIT;

class CONTENT
{
public:
	CONTENT(std::ifstream& file);

	CLINES_CIT get_line_it();
	CHARS_CIT get_char_it();

	int get_x();
	int get_y();

	CLINES_CIT get_first_line_it();
	CLINES_CIT get_last_line_it();

	CHARS_CIT get_contents_end();
	CHARS_CIT get_contents_begin();

	bool is_cursor_at_end();
	bool is_cursor_at_line_end();

	void handle_append(int ch);
	char handle_backspace();
	LeftRightCase handle_left();
	LeftRightCase handle_right();

	void up();
	void down();
	void left(int n);
	void right(int n);

	std::string get_contents_string();

	void log_state();
private:
	CLINES contents;
	TABS tabs;

	int x;
	CLINES_IT y_it;
	CHARS_IT x_it;

	int remove_tab(CLINES_CIT y_it, int x);
	void get_tab_spaces(CLINES_CIT y_it, int x);
	void change_tab_spaces(CLINES_CIT y_it, int x, int change);

	bool is_tab_start(CLINES_CIT y_it, int x);
	bool is_tab_end(CLINES_CIT y_it, int x);
	bool is_tab_stopped(CLINES_CIT y_it, int tab_stop);

	void handle_append(int ch, CLINES_CIT y_it, CHARS_CIT x_it, int* x);
	void handle_append_line(CLINES_CIT y_it, CHARS_CIT x_it, int* x);
	void handle_append_tab(CLINES_CIT y_it, CHARS_CIT x_it, int* x);
	void handle_append_normal(char ch, CLINES_CIT y_it, CHARS_CIT x_it, int* x);

	void handle_backspace_line();
	void handle_backspace_tab();
	char handle_backspace_normal();

	bool is_tab_stop();
	int tab_stop(int x);

	std::string char_it_str(CHARS_CIT x_it, CLINES_CIT y_it);
};

class SCREEN_ITERATOR
{
public:
	SCREEN_ITERATOR(
		CHARS_CIT ch_it,
		CLINES_CIT line_it,
		CHARS_CIT last_char);

	bool is_at_end();
	char next();
private:
	CHARS_CIT ch_it;
	CLINES_CIT line_it;
	CHARS_CIT last_char;
};

struct LINE_ROWS
{
	CLINES_CIT line;
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

class SCREEN_CHANGE;

class CSCREEN
{
private:
	ROWS rows;
	CONTENT content_state;

	int max_rows;
	int max_cols;

	int rows_filled;
	int x;
	int y;
	int line_ind;
	int line_y;

	void append(int line_ind, int num_chars, SCREEN_CHANGE& change);
	void append_line(CLINES_CIT line, int line_ind, SCREEN_CHANGE& change);
	void backspace(int line_ind, int num_chars, SCREEN_CHANGE& change);
	void backspace_line(int line_ind, SCREEN_CHANGE& change);
	void merge_with_next_line(int line_ind, SCREEN_CHANGE& change);
	void scroll_up(SCREEN_CHANGE& change);
	void scroll_down(SCREEN_CHANGE& change);

	void add_row();
	void remove_row();

	CHARS_CIT get_first_char();
	CHARS_CIT get_last_char();
	CLINES_CIT get_first_line();

	int get_rows_in_line(CLINES_CIT line);
	int get_char_offset(CLINES_CIT line);

	void refresh_x();
	int get_max_x_on_row();

	std::string char_it_str(CHARS_CIT x_it, CLINES_CIT y_it);
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

	void commit(int rows_change);

	void log_start(std::string func);
	void log_end();
};

class SCREEN_CHANGE
{
public:
	SCREEN_CHANGE(std::string func, CSCREEN* screen);
	~SCREEN_CHANGE();

	CSCREEN* screen;
	int rows_change;

	bool rows_changed();
};

#endif
