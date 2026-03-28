#ifndef VECTOR_CHARS_H
#define VECTOR_CHARS_H

#include <vector>
#include <iterator>
#include <cstddef>

class VECTOR_CHARS
{
public:
	using iterator = std::vector<char>::iterator;
	using const_iterator = std::vector<char>::const_iterator;

	iterator insert(iterator pos, char ch)
	{
		auto idx = pos - chars.begin();
		chars.insert(pos, ch);
		return chars.begin() + idx + 1;
	}

	iterator erase(iterator pos)
	{
		return chars.erase(pos);
	}

	iterator begin() { return chars.begin(); }
	iterator end()   { return chars.end(); }

	const_iterator begin() const { return chars.begin(); }
	const_iterator end()   const { return chars.end(); }

	size_t size()  const { return chars.size(); }
	bool   empty() const { return chars.empty(); }

	VECTOR_CHARS create_sibling() const { return VECTOR_CHARS(); }

private:
	std::vector<char> chars;
};

#endif
