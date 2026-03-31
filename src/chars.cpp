#include "chars.h"

// --- iterator ---

CHARS::iterator::iterator(std::list<BUFFER_NODE>::iterator node_it,
	size_t buffer_index, std::list<BUFFER_NODE>* nodes_ptr,
	const std::vector<char>* buffer)
	: node_it(node_it), buffer_index(buffer_index), nodes_ptr(nodes_ptr),
	  buffer(buffer) {}

CHARS::iterator::reference CHARS::iterator::operator*() const
{
	return (*buffer)[buffer_index];
}

CHARS::iterator::pointer CHARS::iterator::operator->() const
{
	return &(*buffer)[buffer_index];
}

CHARS::iterator& CHARS::iterator::operator++()
{
	buffer_index++;
	if (buffer_index == node_it->offset + node_it->len)
	{
		node_it++;
		if (node_it != nodes_ptr->end())
			buffer_index = node_it->offset;
		else
			buffer_index = 0;
	}
	return *this;
}

CHARS::iterator& CHARS::iterator::operator--()
{
	if (node_it == nodes_ptr->end())
	{
		node_it--;
		buffer_index = node_it->offset + node_it->len - 1;
		return *this;
	}

	if (buffer_index == node_it->offset)
	{
		node_it--;
		buffer_index = node_it->offset + node_it->len - 1;
		return *this;
	}

	buffer_index--;
	return *this;
}

bool CHARS::iterator::operator==(const iterator& other) const
{
	return node_it == other.node_it && buffer_index == other.buffer_index;
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

void CHARS::insert(iterator& pos, char ch)
{
	buffer->push_back(ch);
	char_count++;
	size_t new_offset = buffer->size() - 1;

	if (pos == end())
	{
		if (!nodes.empty() && nodes.back().offset + nodes.back().len == new_offset)
		{
			nodes.back().len++;
			return;
		}
		nodes.push_back(BUFFER_NODE{new_offset, 1});
		return;
	}

	if (pos.buffer_index == pos.node_it->offset)
	{
		nodes.insert(pos.node_it, BUFFER_NODE{new_offset, 1});
		return;
	}

	// Middle of node — split
	// Current node becomes first half, insert new char + second half after
	size_t chars_before = pos.buffer_index - pos.node_it->offset;
	size_t original_offset = pos.node_it->offset;
	size_t original_len = pos.node_it->len;

	pos.node_it->len = chars_before;

	auto new_char_it = nodes.insert(std::next(pos.node_it),
		BUFFER_NODE{new_offset, 1});
	auto second_half_it = nodes.insert(std::next(new_char_it),
		BUFFER_NODE{original_offset + chars_before, original_len - chars_before});

	pos.node_it = second_half_it;
}

void CHARS::erase(iterator& next)
{
	char_count--;
	auto prev = next;
	--prev;

	size_t offset_in_node = prev.buffer_index - prev.node_it->offset;

	if (prev.node_it->len == 1)
	{
		nodes.erase(prev.node_it);
		return;
	}

	if (offset_in_node == prev.node_it->len - 1)
	{
		prev.node_it->len--;
		return;
	}

	if (offset_in_node == 0)
	{
		prev.node_it->offset++;
		prev.node_it->len--;
		return;
	}

	// Middle — split
	// Current node becomes first half, insert second half after
	size_t original_len = prev.node_it->len;

	prev.node_it->len = offset_in_node;

	auto second_half_it = nodes.insert(std::next(prev.node_it),
		BUFFER_NODE{prev.buffer_index + 1, original_len - offset_in_node - 1});

	if (next.node_it == prev.node_it)
		next.node_it = second_half_it;
}

CHARS::iterator CHARS::begin()
{
	if (nodes.empty())
	{
		return end();
	}
	return iterator(nodes.begin(), nodes.begin()->offset, &nodes, buffer.get());
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
	return const_iterator(mutable_nodes.begin(), mutable_nodes.begin()->offset, &mutable_nodes, buffer.get());
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
