#include <cstdint>
#include "content.h"
#include "tabs.h"

#ifndef POSITION_H
#define POSITION_H

class POSITION
{
public:
	POSITION(CONTENT& content);
	POSITION(CONTENT& content, LINE_IT line_it, int chars_x = 0);
	POSITION(const POSITION& other);

	CONTENT& content;
	LINE_IT line_it;
	CHAR_IT char_it;
	TABS_IT tabs_it;

	int chars_x;
	int spaces_x;
	int tabs_x;

	char get_char() const;

	bool is_first_line() const;
	bool is_at_line_start() const;
	bool is_at_contents_start() const;

	bool is_last_line() const;
	bool is_at_line_end() const;
	bool is_at_contents_end() const;

	bool is_tab() const;
	bool is_after_tab() const;

	virtual char next();
	virtual char prev();

	void reset(const POSITION& other);

	bool operator==(const POSITION& other) const;
	bool operator!=(const POSITION& other) const;
};

#endif
