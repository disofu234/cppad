#ifndef CHARS_H
#define CHARS_H

#include <vector>
#include <list>
#include <memory>
#include <iterator>
#include <cstddef>

struct BUFFER_NODE
{
	size_t offset;
	size_t len;
};

class CHARS
{
public:
	class iterator
	{
	public:
		using iterator_category = std::bidirectional_iterator_tag;
		using value_type = char;
		using difference_type = std::ptrdiff_t;
		using pointer = const char*;
		using reference = const char&;

		iterator();
		iterator(std::list<BUFFER_NODE>::iterator node_it, size_t char_offset,
			std::list<BUFFER_NODE>* nodes_ptr, std::shared_ptr<std::vector<char>> buffer);

		reference operator*() const;
		pointer operator->() const;

		iterator& operator++();
		iterator& operator--();

		bool operator==(const iterator& other) const;
		bool operator!=(const iterator& other) const;

	private:
		std::list<BUFFER_NODE>::iterator node_it;
		size_t char_offset;
		std::list<BUFFER_NODE>* nodes_ptr;
		std::shared_ptr<std::vector<char>> buffer;

		friend class CHARS;
	};

	using const_iterator = iterator;

	CHARS();
	CHARS(std::shared_ptr<std::vector<char>> buf);

	iterator insert(iterator pos, char ch);
	iterator erase(iterator pos);

	iterator begin();
	iterator end();
	const_iterator begin() const;
	const_iterator end() const;

	size_t size() const;
	bool empty() const;

	CHARS create_sibling() const;
	std::shared_ptr<std::vector<char>> get_buffer() const;

private:
	std::shared_ptr<std::vector<char>> buffer;
	std::list<BUFFER_NODE> nodes;
	size_t char_count;
};

#endif
