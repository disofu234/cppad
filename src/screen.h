#ifndef SCREEN_H
#define SCREEN_H

#include "position.h"

class CSCREEN
{
public:
	CSCREEN(CONTENT& content, int rows, int cols);

	CONTENT& content;

	FIRST_POSITION first;

	std::string print();

	int get_rows() const;
	int get_cols() const;
	int get_spaces_in_last_row(LINE_CIT line_it) const;

	void scroll_up();
	void scroll_down();

private:
	int rows;
	int cols;
};

#endif
