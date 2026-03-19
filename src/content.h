#include <list>
#include <iterator>
#include <iostream>
#include <sstream>
#include <string>
#include <memory>
#include <deque>
#include "tabs.h"
#include "chars.h"

#ifndef CONTENT_H
#define CONTENT_H

class LINE
{
public:
	LINE() = default;
	LINE(std::shared_ptr<std::deque<char>> buf);
	~LINE() = default;

	CHARS chars;
	TABS tabs;

	int size() const;
};

typedef std::list<LINE> CONTENT;
typedef CHARS::iterator CHAR_IT;
typedef CHARS::const_iterator CHAR_CIT;
typedef CONTENT::iterator LINE_IT;
typedef CONTENT::const_iterator LINE_CIT;

void initialize_content(CONTENT& content, std::istream& input);
std::string get_content_string(CONTENT& content);

#endif