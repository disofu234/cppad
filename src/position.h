#include <cstdint>
#include "content.h"
#include "tabs.h"

#ifndef POSITION_H
#define POSITION_H

template<typename CHARS_T>
class POSITION_T
{
public:
	POSITION_T(CONTENT_T<CHARS_T>& content);
	POSITION_T(CONTENT_T<CHARS_T>& content,
		typename CONTENT_T<CHARS_T>::iterator line_it, int chars_x = 0);
	POSITION_T(const POSITION_T<CHARS_T>& other);

	CONTENT_T<CHARS_T>& content;
	typename CONTENT_T<CHARS_T>::iterator line_it;
	typename CHARS_T::iterator char_it;
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

	void reset(const POSITION_T<CHARS_T>& other);

	bool operator==(const POSITION_T<CHARS_T>& other) const;
	bool operator!=(const POSITION_T<CHARS_T>& other) const;
};

// Backward-compatible alias
using POSITION = POSITION_T<CHARS>;

#include "position.tpp"

#endif
