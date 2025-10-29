#ifndef CONTENT_CURSOR_H
#define CONTENT_CURSOR_H

#include "position.h"

struct RIGHT
{
	char ch;
	int width;
	int scrolls;
};

struct LEFT
{
	char ch;
	int width;
	int scrolls;
};

struct INSERT
{
	int width;
};

struct BACKSPACE
{
	char ch;
	int width;
};

class CONTENT_CURSOR : public POSITION
{
public:
	CONTENT_CURSOR(CONTENT& content);
	CONTENT_CURSOR(CONTENT& content, LINE_IT line_it, int chars_x = 0);

	INSERT insert(char ch);
	BACKSPACE backspace();
	LEFT left();
	RIGHT right();

private:
	void insert_line();
	void backspace_line();
};

#endif
