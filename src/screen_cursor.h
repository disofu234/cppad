#ifndef SCREEN_CURSOR_H
#define SCREEN_CURSOR_H

#include "screen.h"
#include "position.h"
#include "content_cursor.h"

class SCREEN_CURSOR
{
public:
	SCREEN_CURSOR(CSCREEN& screen);
	SCREEN_CURSOR(CSCREEN& screen, int first_row, int target_x, int target_y);

	void insert(char ch);
	void backspace();
	bool right();
	bool left();

	int get_x() const;
	int get_y() const;

private:
	int x;
	int y;

	int cols;
	int rows;

	CSCREEN& screen;
	CONTENT_CURSOR cc;
};

#endif
