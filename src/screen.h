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

	void scroll_up();
	void scroll_down();

private:
	int rows;
	int cols;
};

#endif
