#include <list>
#include <iterator>
#include <iostream>
#include <sstream>
#include <string>
#include "tabs.h"
#include "chars.h"

#ifndef CONTENT_H
#define CONTENT_H

template<typename CHARS_T>
class LINE_T
{
public:
	LINE_T() = default;
	LINE_T(CHARS_T chars_) : chars(std::move(chars_)) {}
	~LINE_T() = default;

	CHARS_T chars;
	TABS tabs;

	int size() const
	{
		return (int)chars.size() - tabs.tab_count() + tabs.spaces();
	}
};

template<typename CHARS_T>
using CONTENT_T = std::list<LINE_T<CHARS_T>>;

template<typename CHARS_T>
using CHAR_IT_T = typename CHARS_T::iterator;

template<typename CHARS_T>
using CHAR_CIT_T = typename CHARS_T::const_iterator;

template<typename CHARS_T>
void initialize_content(CONTENT_T<CHARS_T>& content, std::istream& input);

template<typename CHARS_T>
std::string get_content_string(CONTENT_T<CHARS_T>& content);

// Backward-compatible aliases
using LINE     = LINE_T<CHARS>;
using CONTENT  = CONTENT_T<CHARS>;
using CHAR_IT  = CHAR_IT_T<CHARS>;
using CHAR_CIT = CHAR_CIT_T<CHARS>;
typedef CONTENT::iterator LINE_IT;
typedef CONTENT::const_iterator LINE_CIT;

#endif
