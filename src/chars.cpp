#include "chars.h"

// --- iterator ---

CHARS::iterator::iterator()
	: node_it(), char_ptr(nullptr), nodes_ptr(nullptr) {}

CHARS::iterator::iterator(std::list<BUFFER_NODE>::iterator node_it,
	const char* char_ptr, std::list<BUFFER_NODE>* nodes_ptr)
	: node_it(node_it), char_ptr(char_ptr), nodes_ptr(nodes_ptr) {}

CHARS::iterator::reference CHARS::iterator::operator*() const
{
	return *char_ptr;
}

CHARS::iterator::pointer CHARS::iterator::operator->() const
{
	return char_ptr;
}

CHARS::iterator& CHARS::iterator::operator++()
{
	char_ptr++;
	if (char_ptr == node_it->ptr + node_it->len)
	{
		node_it++;
		if (node_it != nodes_ptr->end())
		{
			char_ptr = node_it->ptr;
		}
		else
		{
			char_ptr = nullptr;
		}
	}
	return *this;
}

CHARS::iterator CHARS::iterator::operator++(int)
{
	iterator tmp = *this;
	++(*this);
	return tmp;
}

CHARS::iterator& CHARS::iterator::operator--()
{
	if (node_it == nodes_ptr->end())
	{
		node_it--;
		char_ptr = node_it->ptr + node_it->len - 1;
		return *this;
	}

	if (char_ptr == node_it->ptr)
	{
		node_it--;
		char_ptr = node_it->ptr + node_it->len - 1;
		return *this;
	}

	char_ptr--;
	return *this;
}

CHARS::iterator CHARS::iterator::operator--(int)
{
	iterator tmp = *this;
	--(*this);
	return tmp;
}

bool CHARS::iterator::operator==(const iterator& other) const
{
	return node_it == other.node_it && char_ptr == other.char_ptr;
}

bool CHARS::iterator::operator!=(const iterator& other) const
{
	return !(*this == other);
}

// --- CHARS ---

CHARS::CHARS()
	: buffer(std::make_shared<std::deque<char>>()), char_count(0) {}

CHARS::CHARS(std::shared_ptr<std::deque<char>> buf)
	: buffer(std::move(buf)), char_count(0) {}

CHARS::iterator CHARS::insert(iterator pos, char ch)
{
	buffer->push_back(ch);
	char_count++;
	const char* new_char_ptr = &buffer->back();

	if (pos == end())
	{
		if (!nodes.empty() && nodes.back().ptr + nodes.back().len == new_char_ptr)
		{
			nodes.back().len++;
			return pos;
		}
		nodes.push_back(BUFFER_NODE{new_char_ptr, 1});
		return pos;
	}

	if (pos.char_ptr == pos.node_it->ptr)
	{
		nodes.insert(pos.node_it, BUFFER_NODE{new_char_ptr, 1});
		return pos;
	}

	// Middle of node — split
	size_t offset = pos.char_ptr - pos.node_it->ptr;
	size_t original_len = pos.node_it->len;

	pos.node_it->len = offset;

	auto second_half_it = nodes.insert(std::next(pos.node_it),
		BUFFER_NODE{pos.node_it->ptr + offset, original_len - offset});

	nodes.insert(second_half_it, BUFFER_NODE{new_char_ptr, 1});

	pos.node_it = second_half_it;
	return pos;
}

CHARS::iterator CHARS::erase(iterator pos)
{
	char_count--;
	size_t offset = pos.char_ptr - pos.node_it->ptr;

	if (pos.node_it->len == 1)
	{
		auto next_node = nodes.erase(pos.node_it);
		if (next_node != nodes.end())
		{
			return iterator(next_node, next_node->ptr, &nodes);
		}
		return end();
	}

	if (offset == pos.node_it->len - 1)
	{
		pos.node_it->len--;
		auto next_node = std::next(pos.node_it);
		if (next_node != nodes.end())
		{
			return iterator(next_node, next_node->ptr, &nodes);
		}
		return end();
	}

	if (offset == 0)
	{
		pos.node_it->ptr++;
		pos.node_it->len--;
		return iterator(pos.node_it, pos.node_it->ptr, &nodes);
	}

	// Middle — split
	auto second_half_it = nodes.insert(std::next(pos.node_it),
		BUFFER_NODE{pos.node_it->ptr + offset + 1, pos.node_it->len - offset - 1});
	pos.node_it->len = offset;

	return iterator(second_half_it, second_half_it->ptr, &nodes);
}

CHARS::iterator CHARS::begin()
{
	if (nodes.empty())
	{
		return end();
	}
	return iterator(nodes.begin(), nodes.begin()->ptr, &nodes);
}

CHARS::iterator CHARS::end()
{
	return iterator(nodes.end(), nullptr, &nodes);
}

CHARS::const_iterator CHARS::begin() const
{
	auto& mutable_nodes = const_cast<std::list<BUFFER_NODE>&>(nodes);
	if (mutable_nodes.empty())
	{
		return const_iterator(mutable_nodes.end(), nullptr, &mutable_nodes);
	}
	return const_iterator(mutable_nodes.begin(), mutable_nodes.begin()->ptr, &mutable_nodes);
}

CHARS::const_iterator CHARS::end() const
{
	auto& mutable_nodes = const_cast<std::list<BUFFER_NODE>&>(nodes);
	return const_iterator(mutable_nodes.end(), nullptr, &mutable_nodes);
}

size_t CHARS::size() const
{
	return char_count;
}

bool CHARS::empty() const
{
	return char_count == 0;
}

std::shared_ptr<std::deque<char>> CHARS::get_buffer() const
{
	return buffer;
}
