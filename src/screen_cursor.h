#ifndef SCREEN_CURSOR_H
#define SCREEN_CURSOR_H

#include "screen.h"
#include "position.h"
#include "content_cursor.h"

struct DOWN
{
	int scrolls;
};

struct UP
{
	int scrolls;
};

class SCREEN_CURSOR
{
public:
	SCREEN_CURSOR(CSCREEN& screen);
	SCREEN_CURSOR(CSCREEN& screen, int target_first_row, int target_x, int target_y);

	void insert(char ch);
	void backspace();
	RIGHT right();
	LEFT left();
	UP up();
	DOWN down();

	int get_x() const;
	int get_y() const;

private:
	int x;
	int y;

	int cols;
	int rows;

	CSCREEN& screen;
	CONTENT_CURSOR cc;

	bool is_at_first_row_in_line() const;
	bool is_at_last_row_in_line() const;

	bool is_at_content_first_row() const;
	bool is_at_content_last_row() const;

	int get_row_in_line() const;
	int get_num_rows_in_line() const;
};

#endif
