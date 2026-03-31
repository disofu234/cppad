#include "chars.h"

// --- iterator ---

CHARS::iterator::iterator(std::list<BUFFER_NODE>::iterator node_it,
	size_t char_offset, std::list<BUFFER_NODE>* nodes_ptr,
	const std::vector<char>* buffer)
	: node_it(node_it), char_offset(char_offset), nodes_ptr(nodes_ptr),
	  buffer(buffer) {}

CHARS::iterator::reference CHARS::iterator::operator*() const
{
	return (*buffer)[node_it->offset + char_offset];
}

CHARS::iterator::pointer CHARS::iterator::operator->() const
{
	return &(*buffer)[node_it->offset + char_offset];
}

CHARS::iterator& CHARS::iterator::operator++()
{
	char_offset++;
	if (char_offset == node_it->len)
	{
		node_it++;
		char_offset = 0;
	}
	return *this;
}

CHARS::iterator& CHARS::iterator::operator--()
{
	if (node_it == nodes_ptr->end())
	{
		node_it--;
		char_offset = node_it->len - 1;
		return *this;
	}

	if (char_offset == 0)
	{
		node_it--;
		char_offset = node_it->len - 1;
		return *this;
	}

	char_offset--;
	return *this;
}

bool CHARS::iterator::operator==(const iterator& other) const
{
	return node_it == other.node_it && char_offset == other.char_offset;
}

bool CHARS::iterator::operator!=(const iterator& other) const
{
	return !(*this == other);
}

// --- CHARS ---

CHARS::CHARS()
	: buffer(std::make_shared<std::vector<char>>()), char_count(0) {}

CHARS::CHARS(std::shared_ptr<std::vector<char>> buf)
	: buffer(std::move(buf)), char_count(0) {}

CHARS::iterator CHARS::insert(iterator pos, char ch)
{
	buffer->push_back(ch);
	char_count++;
	size_t new_offset = buffer->size() - 1;

	if (pos == end())
	{
		if (!nodes.empty() && nodes.back().offset + nodes.back().len == new_offset)
		{
			nodes.back().len++;
			return pos;
		}
		nodes.push_back(BUFFER_NODE{new_offset, 1});
		return pos;
	}

	if (pos.char_offset == 0)
	{
		nodes.insert(pos.node_it, BUFFER_NODE{new_offset, 1});
		return pos;
	}

	// Middle of node — split
	size_t offset = pos.char_offset;
	size_t original_len = pos.node_it->len;

	pos.node_it->len = offset;

	auto second_half_it = nodes.insert(std::next(pos.node_it),
		BUFFER_NODE{pos.node_it->offset + offset, original_len - offset});

	nodes.insert(second_half_it, BUFFER_NODE{new_offset, 1});

	pos.node_it = second_half_it;
	pos.char_offset = 0;
	return pos;
}

CHARS::iterator CHARS::erase(iterator pos)
{
	char_count--;
	size_t offset = pos.char_offset;

	if (pos.node_it->len == 1)
	{
		auto next_node = nodes.erase(pos.node_it);
		if (next_node != nodes.end())
		{
			return iterator(next_node, 0, &nodes, buffer.get());
		}
		return end();
	}

	if (offset == pos.node_it->len - 1)
	{
		pos.node_it->len--;
		auto next_node = std::next(pos.node_it);
		if (next_node != nodes.end())
		{
			return iterator(next_node, 0, &nodes, buffer.get());
		}
		return end();
	}

	if (offset == 0)
	{
		pos.node_it->offset++;
		pos.node_it->len--;
		return iterator(pos.node_it, 0, &nodes, buffer.get());
	}

	// Middle — split
	auto second_half_it = nodes.insert(std::next(pos.node_it),
		BUFFER_NODE{pos.node_it->offset + offset + 1, pos.node_it->len - offset - 1});
	pos.node_it->len = offset;

	return iterator(second_half_it, 0, &nodes, buffer.get());
}

CHARS::iterator CHARS::begin()
{
	if (nodes.empty())
	{
		return end();
	}
	return iterator(nodes.begin(), 0, &nodes, buffer.get());
}

CHARS::iterator CHARS::end()
{
	return iterator(nodes.end(), 0, &nodes, buffer.get());
}

CHARS::const_iterator CHARS::begin() const
{
	auto& mutable_nodes = const_cast<std::list<BUFFER_NODE>&>(nodes);
	if (mutable_nodes.empty())
	{
		return const_iterator(mutable_nodes.end(), 0, &mutable_nodes, buffer.get());
	}
	return const_iterator(mutable_nodes.begin(), 0, &mutable_nodes, buffer.get());
}

CHARS::const_iterator CHARS::end() const
{
	auto& mutable_nodes = const_cast<std::list<BUFFER_NODE>&>(nodes);
	return const_iterator(mutable_nodes.end(), 0, &mutable_nodes, buffer.get());
}

size_t CHARS::size() const
{
	return char_count;
}

bool CHARS::empty() const
{
	return char_count == 0;
}

CHARS CHARS::create_sibling() const
{
	return CHARS(buffer);
}

std::shared_ptr<std::vector<char>> CHARS::get_buffer() const
{
	return buffer;
}
