#ifndef LIST_CHARS_H
#define LIST_CHARS_H

#include <list>
#include <iterator>
#include <cstddef>

class LIST_CHARS
{
public:
	using iterator = std::list<char>::iterator;
	using const_iterator = std::list<char>::const_iterator;

	void insert(iterator& pos, char ch)
	{
		chars.insert(pos, ch);
	}

	void erase(const iterator& pos, iterator& next)
	{
		(void)next;
		chars.erase(pos);
	}

	iterator begin() { return chars.begin(); }
	iterator end()   { return chars.end(); }

	const_iterator begin() const { return chars.begin(); }
	const_iterator end()   const { return chars.end(); }

	size_t size()  const { return chars.size(); }
	bool   empty() const { return chars.empty(); }

	LIST_CHARS create_sibling() const { return LIST_CHARS(); }

private:
	std::list<char> chars;
};

#endif
