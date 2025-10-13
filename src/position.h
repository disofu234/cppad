#include "content_cursor.h"
#include <cstdint>

#ifndef POSITION_H
#define POSITION_H

class CSCREEN;

class POSITION
{
public:
	POSITION(CONTENT& content);
	POSITION(CONTENT& content, LINE_IT line_it, int x = 0);

	POSITION(const POSITION& other);
	void reset(const CONTENT_CURSOR& cc);

	char next();
	char prev();

	int distance_to(const POSITION& other) const;

	bool is_at_contents_end() const;
	bool is_at_line_start() const;

	bool operator==(const POSITION& other) const;
	bool operator!=(const POSITION& other) const;

protected:
	CONTENT& content;
	CHAR_IT char_it;
	TABS_IT tabs_it;
	int tabs_x;
	int x;

	char get_char() const;

	bool is_first_line() const;
	bool is_at_contents_start() const;

	bool is_last_line() const;
	bool is_at_line_end() const;

	int max_tabs_x() const;

public:
	LINE_IT line_it;
};

class FIRST_POSITION : public POSITION
{
public:
	FIRST_POSITION(CSCREEN& screen);

	void next_row();
	void prev_row();
	void set(LINE_IT line_it, int x = 0);

private:
	CSCREEN& screen;
	int get_spaces_in_last_row() const;
};

#endif
