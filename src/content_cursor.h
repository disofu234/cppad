#ifndef CURSOR_H
#define CURSOR_H

#include <list>
#include <iterator>
#include <iostream>
#include <sstream>
#include <string>
#include "tabs.h"

typedef std::list<char> CHARS;

class LINE
{
public:
	LINE() = default;
    ~LINE() = default;

	CHARS chars;
	TABS tabs;

	int size() const;
};

typedef std::list<LINE> CONTENT;
typedef CHARS::iterator CHAR_IT;
typedef CHARS::const_iterator CHAR_CIT;
typedef CONTENT::iterator LINE_IT;
typedef CONTENT::const_iterator LINE_CIT;

void initialize_content(CONTENT& content, std::istream& input);
std::string get_content_string(CONTENT& content);

struct RIGHT
{
	char ch;
	int width;
};

struct LEFT
{
	char ch;
	int width;
};

struct INSERT
{
	int width;
	LINE_IT* new_line;
};

class CONTENT_CURSOR
{
public:
	CONTENT_CURSOR(CONTENT& content);
	CONTENT_CURSOR(CONTENT& content, LINE_IT line_it, int x = 0);

	INSERT insert(char ch);
	char backspace();
	LEFT left();
	RIGHT right();

	LINE_IT get_line_it() const;
	CHAR_IT get_char_it() const;
	TABS_IT get_tabs_it() const;
	int get_x() const;
	int get_tabs_x() const;

	char get_char() const;

	bool is_first_line() const;
	bool is_at_line_start() const;
	bool is_at_contents_start() const;

	bool is_last_line() const;
	bool is_at_line_end() const;
	bool is_at_contents_end() const;

private:
	CONTENT& content;
	LINE_IT line_it;
	CHAR_IT char_it;
	int x;

	TABS_IT tabs_it;
	int tabs_x;

	void insert_line();
	void backspace_line();

	char next_ch();
	char prev_ch();
	char next_line();
	char prev_line();

	bool is_tab_start() const;
	bool is_tab_end() const;
};

#endif
