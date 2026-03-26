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

template<typename CHARS_T>
class CONTENT_CURSOR_T : public POSITION_T<CHARS_T>
{
public:
	CONTENT_CURSOR_T(CONTENT_T<CHARS_T>& content);
	CONTENT_CURSOR_T(CONTENT_T<CHARS_T>& content,
		typename CONTENT_T<CHARS_T>::iterator line_it, int chars_x = 0);

	INSERT insert(char ch);
	BACKSPACE backspace();
	LEFT left();
	RIGHT right();

private:
	void insert_line();
	void backspace_line();
};

// Backward-compatible alias
using CONTENT_CURSOR = CONTENT_CURSOR_T<CHARS>;

#include "content_cursor.tpp"

#endif
