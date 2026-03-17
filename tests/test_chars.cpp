#include <gtest/gtest.h>
#include "content_cursor.h"

class CharsTest : public ::testing::Test
{
protected:
	CONTENT content;
};

// --- Initialization via initialize_content ---

TEST_F(CharsTest, InitializeEmpty)
{
	std::istringstream input("");
	initialize_content(content, input);

	EXPECT_EQ(get_content_string(content), "");
	EXPECT_EQ(content.begin()->chars.size(), 0);
	EXPECT_TRUE(content.begin()->chars.empty());
}

TEST_F(CharsTest, InitializeSingleChar)
{
	std::istringstream input("a");
	initialize_content(content, input);

	EXPECT_EQ(get_content_string(content), "a");
	EXPECT_EQ(content.begin()->chars.size(), 1);
}

TEST_F(CharsTest, InitializeMultiCharSingleLine)
{
	std::istringstream input("hello");
	initialize_content(content, input);

	EXPECT_EQ(get_content_string(content), "hello");
	EXPECT_EQ(content.begin()->chars.size(), 5);
}

TEST_F(CharsTest, InitializeMultipleLines)
{
	std::istringstream input("abc\ndef\nghi");
	initialize_content(content, input);

	EXPECT_EQ(get_content_string(content), "abc\ndef\nghi");
}

// --- Insert at end of buffer (sequential typing) ---

TEST_F(CharsTest, InsertSequentialAtEnd)
{
	std::istringstream input("");
	initialize_content(content, input);
	CONTENT_CURSOR cursor(content);

	cursor.insert('a');
	cursor.insert('b');
	cursor.insert('c');

	EXPECT_EQ(get_content_string(content), "abc");
	EXPECT_EQ(content.begin()->chars.size(), 3);
}

TEST_F(CharsTest, InsertIntoEmptyContent)
{
	std::istringstream input("");
	initialize_content(content, input);
	CONTENT_CURSOR cursor(content);

	cursor.insert('x');

	EXPECT_EQ(get_content_string(content), "x");
	EXPECT_EQ(content.begin()->chars.size(), 1);
	EXPECT_FALSE(content.begin()->chars.empty());
}

// --- Insert at beginning of current buffer node ---

TEST_F(CharsTest, InsertAtBeginningOfLine)
{
	std::istringstream input("bc");
	initialize_content(content, input);
	CONTENT_CURSOR cursor(content);

	cursor.insert('a');

	EXPECT_EQ(get_content_string(content), "abc");
	EXPECT_EQ(content.begin()->chars.size(), 3);
}

// --- Insert in the middle of a buffer node ---

TEST_F(CharsTest, InsertInMiddleOfLine)
{
	std::istringstream input("ac");
	initialize_content(content, input);
	CONTENT_CURSOR cursor(content, content.begin(), 1);

	cursor.insert('b');

	EXPECT_EQ(get_content_string(content), "abc");
	EXPECT_EQ(content.begin()->chars.size(), 3);
}

TEST_F(CharsTest, InsertMultipleInMiddle)
{
	std::istringstream input("ad");
	initialize_content(content, input);

	CONTENT_CURSOR cursor(content, content.begin(), 1);
	cursor.insert('b');
	cursor.insert('c');

	EXPECT_EQ(get_content_string(content), "abcd");
}

// --- Insert causing line split ---

TEST_F(CharsTest, InsertNewline)
{
	std::istringstream input("abcd");
	initialize_content(content, input);
	CONTENT_CURSOR cursor(content, content.begin(), 2);

	cursor.insert('\n');

	EXPECT_EQ(get_content_string(content), "ab\ncd");
}

// --- Erase at end of buffer node ---

TEST_F(CharsTest, EraseSingleChar)
{
	std::istringstream input("a");
	initialize_content(content, input);
	CONTENT_CURSOR cursor(content, content.begin(), 1);

	BACKSPACE result = cursor.backspace();

	EXPECT_EQ(result.ch, 'a');
	EXPECT_EQ(get_content_string(content), "");
	EXPECT_EQ(content.begin()->chars.size(), 0);
	EXPECT_TRUE(content.begin()->chars.empty());
}

TEST_F(CharsTest, EraseLastChar)
{
	std::istringstream input("abc");
	initialize_content(content, input);
	CONTENT_CURSOR cursor(content, content.begin(), 3);

	cursor.backspace();

	EXPECT_EQ(get_content_string(content), "ab");
	EXPECT_EQ(content.begin()->chars.size(), 2);
}

// --- Erase at beginning of buffer node ---

TEST_F(CharsTest, EraseFirstChar)
{
	std::istringstream input("abc");
	initialize_content(content, input);
	CONTENT_CURSOR cursor(content, content.begin(), 1);

	cursor.backspace();

	EXPECT_EQ(get_content_string(content), "bc");
	EXPECT_EQ(content.begin()->chars.size(), 2);
}

// --- Erase in the middle of a buffer node ---

TEST_F(CharsTest, EraseMiddleChar)
{
	std::istringstream input("abc");
	initialize_content(content, input);
	CONTENT_CURSOR cursor(content, content.begin(), 2);

	cursor.backspace();

	EXPECT_EQ(get_content_string(content), "ac");
	EXPECT_EQ(content.begin()->chars.size(), 2);
}

// --- Multiple erasures ---

TEST_F(CharsTest, EraseAllChars)
{
	std::istringstream input("abc");
	initialize_content(content, input);
	CONTENT_CURSOR cursor(content, content.begin(), 3);

	cursor.backspace();
	cursor.backspace();
	cursor.backspace();

	EXPECT_EQ(get_content_string(content), "");
	EXPECT_TRUE(content.begin()->chars.empty());
}

// --- Mixed insert and erase ---

TEST_F(CharsTest, InsertThenErase)
{
	std::istringstream input("");
	initialize_content(content, input);
	CONTENT_CURSOR cursor(content);

	cursor.insert('a');
	cursor.insert('b');
	cursor.insert('c');
	cursor.backspace();

	EXPECT_EQ(get_content_string(content), "ab");
}

TEST_F(CharsTest, EraseAndReinsert)
{
	std::istringstream input("abc");
	initialize_content(content, input);
	CONTENT_CURSOR cursor(content, content.begin(), 2);

	cursor.backspace();
	cursor.insert('x');

	EXPECT_EQ(get_content_string(content), "axc");
}

// --- Multi-line operations ---

TEST_F(CharsTest, InsertLineAndBackspaceLine)
{
	std::istringstream input("abcd");
	initialize_content(content, input);
	CONTENT_CURSOR cursor(content, content.begin(), 2);

	cursor.insert('\n');
	EXPECT_EQ(get_content_string(content), "ab\ncd");

	cursor.backspace();
	EXPECT_EQ(get_content_string(content), "abcd");
}

TEST_F(CharsTest, MultiLineInsertAndErase)
{
	std::istringstream input("ab\ncd");
	initialize_content(content, input);

	LINE_IT second_line = std::next(content.begin());
	CONTENT_CURSOR cursor(content, second_line, 1);

	cursor.insert('x');
	EXPECT_EQ(get_content_string(content), "ab\ncxd");

	cursor.backspace();
	EXPECT_EQ(get_content_string(content), "ab\ncd");
}

// --- Buffer sharing across lines ---

TEST_F(CharsTest, NewLinesShareBuffer)
{
	std::istringstream input("abcd");
	initialize_content(content, input);
	CONTENT_CURSOR cursor(content, content.begin(), 2);

	cursor.insert('\n');

	LINE_IT first = content.begin();
	LINE_IT second = std::next(first);
	EXPECT_EQ(first->chars.get_buffer(), second->chars.get_buffer());
}

// --- Iterator navigation ---

TEST_F(CharsTest, IteratorTraversal)
{
	std::istringstream input("abc");
	initialize_content(content, input);

	std::string result;
	for (auto it = content.begin()->chars.begin();
		it != content.begin()->chars.end(); ++it)
	{
		result += *it;
	}
	EXPECT_EQ(result, "abc");
}

TEST_F(CharsTest, IteratorReverseTraversal)
{
	std::istringstream input("abc");
	initialize_content(content, input);

	auto it = content.begin()->chars.end();
	std::string result;
	while (it != content.begin()->chars.begin())
	{
		--it;
		result += *it;
	}
	EXPECT_EQ(result, "cba");
}

TEST_F(CharsTest, StdPrevWorks)
{
	std::istringstream input("abc");
	initialize_content(content, input);

	auto it = content.begin()->chars.end();
	auto prev_it = std::prev(it);
	EXPECT_EQ(*prev_it, 'c');

	prev_it = std::prev(prev_it);
	EXPECT_EQ(*prev_it, 'b');
}

// --- Erase causing node removal ---

TEST_F(CharsTest, EraseUntilNodeRemoved)
{
	std::istringstream input("a");
	initialize_content(content, input);
	CONTENT_CURSOR cursor(content, content.begin(), 1);

	cursor.backspace();

	EXPECT_TRUE(content.begin()->chars.empty());
	EXPECT_EQ(content.begin()->chars.begin(), content.begin()->chars.end());
}

// --- Insert after erase in fragmented state ---

TEST_F(CharsTest, InsertAfterMiddleErase)
{
	std::istringstream input("abcde");
	initialize_content(content, input);

	CONTENT_CURSOR cursor(content, content.begin(), 3);
	cursor.backspace();
	EXPECT_EQ(get_content_string(content), "abde");

	cursor.insert('x');
	EXPECT_EQ(get_content_string(content), "abxde");
}
